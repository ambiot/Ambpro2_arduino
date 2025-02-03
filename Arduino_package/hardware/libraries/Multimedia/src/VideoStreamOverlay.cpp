#include <Arduino.h>
#include "VideoStreamOverlay.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "osd_render.h"

#ifdef __cplusplus
}
#endif

VideoStreamOverlay OSD;

void VideoStreamOverlay::configVideo(int ch, VideoSetting &config)
{
    ch_enable[ch] = 1;
    ch_width[ch] = config._w;
    ch_height[ch] = config._h;

    // printf("\r\n[INFO] ch_enable %d  %d  %d\n", ch_enable[0], ch_enable[1], ch_enable[2]);
    // printf("\r\n[INFO] character_width %d  %d  %d\n", character_width[0], character_width[1], character_width[2]);
    // printf("\r\n[INFO] character_height %d  %d  %d\n", character_height[0], character_height[1], character_height[2]);
    // printf("\r\n[INFO] ch_width %d  %d  %d\n", ch_width[0], ch_width[1], ch_width[2]);
    // printf("\r\n[INFO] ch_height %d  %d  %d\n", ch_height[0], ch_height[1], ch_height[2]);
}

void VideoStreamOverlay::configTextSize(int ch, int text_width, int text_height)
{
    character_width[ch] = text_width;
    character_height[ch] = text_height;
}

void VideoStreamOverlay::createBitmap(int ch, int idx)
{
    canvas_create_bitmap(ch, idx, RTS_OSD2_BLK_FMT_RGBA2222);
}

void VideoStreamOverlay::begin(void)
{
    osd_render_dev_init(ch_enable, character_width, character_height);
    osd_render_task_start(ch_enable, ch_width, ch_height);
}

void VideoStreamOverlay::end(void)
{
    osd_render_dev_deinit_all();
    osd_render_task_stop();
}

void VideoStreamOverlay::endChannel(int ch)
{
    ch_enable[ch] = 0;
    osd_render_dev_deinit(ch);

    for (int i = 0; i < OSD_OBJ_MAX_CH; i++) {
        if (ch_enable[ch]) {
            break;
        }
        end();    // Check for active channels, if no channels active, stop task
    }
}

int VideoStreamOverlay::getTextHeight(int ch)
{
    return character_height[ch];
}

int VideoStreamOverlay::getTextWidth(int ch)
{
    return character_width[ch];
}

uint32_t VideoStreamOverlay::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    return (ARGB(alpha, red, green, blue));
}

void VideoStreamOverlay::drawLine(int ch, int xmin, int ymin, int xmax, int ymax, int line_width, uint32_t color, int idx)
{
    canvas_set_line(ch, idx, xmin, ymin, xmax, ymax, line_width, color);
}

void VideoStreamOverlay::drawPoint(int ch, int xmin, int ymin, int point_width, uint32_t color, int idx)
{
    canvas_set_point(ch, idx, xmin, ymin, point_width, color);
}

void VideoStreamOverlay::drawRect(int ch, int xmin, int ymin, int xmax, int ymax, int line_width, uint32_t color, int idx)
{
    canvas_set_rect(ch, idx, xmin, ymin, xmax, ymax, line_width, color);
}

void VideoStreamOverlay::drawText(int ch, int xmin, int ymin, const char *text_string, uint32_t color, int idx)
{
    canvas_set_text(ch, idx, xmin, ymin, (char *)text_string, color);
}

void VideoStreamOverlay::update(int ch, int idx)
{
    canvas_update(ch, idx, 1);
}

void VideoStreamOverlay::update(int ch, int ready_to_update, int idx)
{
    canvas_update(ch, idx, ready_to_update);
}
