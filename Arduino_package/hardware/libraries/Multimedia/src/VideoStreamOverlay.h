#ifndef __VIDEOSTREAMOVERLAY_H__
#define __VIDEOSTREAMOVERLAY_H__

#include "VideoStream.h"
#include "canvas.h"

// DEFAULT is previously defined in wiring_constants.h
#ifdef DEFAULT
#undef DEFAULT
#endif

#include "osd_api.h"

#define OSD_COLOR_RED     ARGB(0xff, 0xff, 0x00, 0x00)
#define OSD_COLOR_BLUE    ARGB(0xff, 0x00, 0x00, 0xff)
#define OSD_COLOR_GREEN   ARGB(0xff, 0x00, 0xff, 0x00)
#define OSD_COLOR_CYAN    ARGB(0xff, 0x00, 0xff, 0xff)
#define OSD_COLOR_MAGENTA ARGB(0xff, 0xff, 0x00, 0xff)
#define OSD_COLOR_YELLOW  ARGB(0xff, 0xff, 0xff, 0x00)
#define OSD_COLOR_WHITE   ARGB(0xff, 0xff, 0xff, 0xff)
#define OSD_COLOR_BLACK   ARGB(0xff, 0x00, 0x00, 0x00)
#define OSD_COLOR_GRAY    ARGB(0xff, 0x7f, 0x7f, 0x7f)
#define OSD_COLOR_ORANGE  ARGB(0xff, 0xf3, 0x70, 0x21)

// Each channel supports 6 OSD layers
#define OSDLAYER0 0
#define OSDLAYER1 1
#define OSDLAYER2 2
#define OSDLAYER3 3
#define OSDLAYER4 4
#define OSDLAYER5 5

class VideoStreamOverlay {
public:
    void configVideo(int ch, VideoSetting &config);
    void configTextSize(int ch, int text_width, int text_height);
    void createBitmap(int ch, int idx = 0);
    void begin(void);
    void end(void);
    void endChannel(int ch);
    int getTextHeight(int ch);
    int getTextWidth(int ch);

    uint32_t color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff);
    void drawLine(int ch, int xmin, int ymin, int xmax, int ymax, int line_width, uint32_t color, int idx = 0);
    void drawPoint(int ch, int xmin, int ymin, int point_width, uint32_t color, int idx = 0);
    void drawRect(int ch, int xmin, int ymin, int xmax, int ymax, int line_width, uint32_t color, int idx = 0);
    void drawText(int ch, int xmin, int ymin, const char *text_string, uint32_t color, int idx = 0);
    void update(int ch, int idx = 0);
    void update(int ch, int ready_to_update, int idx = 0);

private:
    int ch_enable[3] = {0};
    int character_width[3] = {16, 16, 16};    // Text character size in pixels
    int character_height[3] = {32, 32, 32};
    int ch_width[3] = {0};    // Video channel resolution
    int ch_height[3] = {0};
};

extern VideoStreamOverlay OSD;

#endif
