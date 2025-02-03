#include "WiFi.h"
#include "StreamIO.h"
#include "VideoStream.h"
#include "RTSP.h"
#include "NNGestureDetection.h"
#include "VideoStreamOverlay.h"

#define CHANNEL   0
#define CHANNELNN 3

// Customised resolution for NN
#define NNWIDTH  192
#define NNHEIGHT 192

VideoSetting config(VIDEO_FHD, 30, VIDEO_H264, 0);
VideoSetting configNN(NNWIDTH, NNHEIGHT, 10, VIDEO_RGB, 0);
NNGestureDetection gesturedetect;
RTSP rtsp;
StreamIO videoStreamer(1, 1);
StreamIO videoStreamerRGBGD(1, 1);

char ssid[] = "Network_SSID5";    // your network SSID (name)
char pass[] = "Password";         // your network password
int status = WL_IDLE_STATUS;

IPAddress ip;
int rtsp_portnum;

void setup()
{
    Serial.begin(115200);

    // Attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);

        // wait 2 seconds for connection:
        delay(2000);
    }

    ip = WiFi.localIP();

    // Configure camera video channels with video format information
    // Adjust the bitrate based on your WiFi network quality
    config.setBitrate(2 * 1024 * 1024);    // Recommend to use 2Mbps for RTSP streaming to prevent network congestion
    Camera.configVideoChannel(CHANNEL, config);
    Camera.configVideoChannel(CHANNELNN, configNN);
    Camera.videoInit();

    // Configure RTSP with corresponding video format information
    rtsp.configVideo(config);
    rtsp.begin();
    rtsp_portnum = rtsp.getPort();

    // Configure Gesture Detection model
    // Select Neural Network(NN) task and models
    gesturedetect.configVideo(configNN);
    gesturedetect.modelSelect(GESTURE_DETECTION, NA_MODEL, NA_MODEL, NA_MODEL, NA_MODEL, NA_MODEL, DEFAULT_PALMDETECT, DEFAULT_HANDLANDMARK);
    gesturedetect.begin();

    // Configure StreamIO object to stream data from video channel to RTSP
    videoStreamer.registerInput(Camera.getStream(CHANNEL));
    videoStreamer.registerOutput(rtsp);
    if (videoStreamer.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }
    // Start data stream from video channel
    Camera.channelBegin(CHANNEL);

    // Configure StreamIO object to stream data from RGB video channel to gesture detection
    videoStreamerRGBGD.registerInput(Camera.getStream(CHANNELNN));
    videoStreamerRGBGD.setStackSize();
    videoStreamerRGBGD.setTaskPriority();
    videoStreamerRGBGD.registerOutput(gesturedetect);
    if (videoStreamerRGBGD.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start video channel for NN
    Camera.channelBegin(CHANNELNN);

    // Start OSD drawing on RTSP video channel
    OSD.configVideo(CHANNEL, config);
    OSD.configTextSize(CHANNEL, 16, 32);
    OSD.begin();
    gesturedetect.drawHandRegion();
}

void loop()
{
    // Do nothing
}
