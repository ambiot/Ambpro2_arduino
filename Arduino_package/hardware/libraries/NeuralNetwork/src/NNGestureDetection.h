#ifndef __NN_GESTUREDETECTION_H__
#define __NN_GESTUREDETECTION_H__

#include "VideoStream.h"
#include "NNModelSelection.h"
#include "osd_api.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "module_vipnn.h"
#include "mmf2_siso.h"
#ifdef __cplusplus
}
#endif

#undef min
#undef max
#include <vector>

#define HAND_LINK_LAYER          0
#define HAND_JOINT_LAYER         1
#define HAND_DETECT_REGION_LAYER 2

#define RTSP_CHANNEL    0
#define RTSP_RESOLUTION VIDEO_FHD
#define RTSP_FPS        30
#define RTSP_GOP        30
#define RTSP_BPS        2 * 1024 * 1024
#define VIDEO_RCMODE    2    // 1: CBR, 2: VBR

#define RTSP_TYPE   VIDEO_H264
#define RTSP_CODEC  AV_CODEC_ID_H264
#define RTSP_WIDTH  1920
#define RTSP_HEIGHT 1080

// NN model config //
#define NN_FPS  10
#define NN_GOP  NN_FPS
#define NN_TYPE VIDEO_RGB
class GestureDetectionResult {
    friend class NNGestureDetection;

public:
    const char* name(void);
    int score(void);
    float xMin(void);
    float xMax(void);
    float yMin(void);
    float yMax(void);
    float xFeature(uint8_t index);
    float yFeature(uint8_t index);

private:
    detobj_t result = {0};
    landmark3d_t landmark = {0};
};

class NNGestureDetection: public NNModelSelection {
public:
    NNGestureDetection(void);
    ~NNGestureDetection(void);

    void configVideo(VideoSetting& config);
    void configThreshold(float confidence_threshold, float nms_threshold);
    void begin(void);
    void end(void);
    void drawHandRegion(void);

    void setResultCallback(void (*fr_callback)(std::vector<GestureDetectionResult>));
    uint16_t getResultCount(void);
    GestureDetectionResult getResult(uint16_t index);
    std::vector<GestureDetectionResult> getResult(void);

private:
    static void drawHandObject(void* p, void* img_param);
    static int fixLandmark(handland_res_t* lm);
    static void handCleanupCallback(TimerHandle_t xTimer);

    static std::vector<GestureDetectionResult> hand_result_vector;
    static void (*GD_user_CB)(std::vector<GestureDetectionResult>);
    static TimerHandle_t osd_cleanup_timer;

    mm_context_t* palmdetect_ctx = NULL;
    mm_context_t* handlandmark_ctx = NULL;
    mm_context_t* video_rgb_ctx = NULL;
    void* palm_hand_siso_ctx = NULL;

    static nn_data_param_t palm_nn_roi;
    static nn_data_param_t handlandmark_nn_roi;

    float pd_confidence_thresh = 0.5;
    float pd_nms_thresh = 0.3;
};
#endif
