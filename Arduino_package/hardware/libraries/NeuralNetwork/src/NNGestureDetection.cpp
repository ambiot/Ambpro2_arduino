#include <Arduino.h>
#include "NNGestureDetection.h"
#include "SD_Model.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "timers.h"
#include "module_vipnn.h"
#include "model_palm_detection.h"
#include "model_hand_landmark.h"
#include "siso_drv.h"
#include "avcodec.h"
#include "vfs.h"
#include "osd_render.h"
#include "roi_delta_qp/roi_delta_qp.h"
#include "VideoStreamOverlay.h"

extern int vipnn_control(void *p, int cmd, int arg);

#ifdef __cplusplus
}
#endif

#undef min
#undef max
#include <vector>

#define LIMIT(x, lower, upper) \
    if (x < lower) \
        x = lower; \
    else if (x > upper) \
        x = upper;

std::vector<GestureDetectionResult> NNGestureDetection::hand_result_vector;
void (*NNGestureDetection::GD_user_CB)(std::vector<GestureDetectionResult>);

TimerHandle_t NNGestureDetection::osd_cleanup_timer = NULL;
nn_data_param_t NNGestureDetection::palm_nn_roi = {0};
nn_data_param_t NNGestureDetection::handlandmark_nn_roi = {0};

NNGestureDetection::NNGestureDetection(void)
{
}

NNGestureDetection::~NNGestureDetection(void)
{
    end();
}

void NNGestureDetection::configVideo(VideoSetting &config)
{
    palm_nn_roi.img.width = config._w;
    palm_nn_roi.img.height = config._h;
    palm_nn_roi.img.rgb = 0;
    palm_nn_roi.img.roi.xmin = 0;
    palm_nn_roi.img.roi.xmax = config._w;
    palm_nn_roi.img.roi.ymin = 0;
    palm_nn_roi.img.roi.ymax = config._h;
    palm_nn_roi.codec_type = AV_CODEC_ID_RGB888;

    handlandmark_nn_roi.img.width = 224;
    handlandmark_nn_roi.img.height = 224;
    handlandmark_nn_roi.img.rgb = 0;
    handlandmark_nn_roi.img.roi.xmin = 0;
    handlandmark_nn_roi.img.roi.xmax = 224;
    handlandmark_nn_roi.img.roi.ymin = 0;
    handlandmark_nn_roi.img.roi.ymax = 224;
    handlandmark_nn_roi.codec_type = AV_CODEC_ID_NN_RAW;
}

void NNGestureDetection::configThreshold(float confidence_threshold, float nms_threshold)
{
    pd_confidence_thresh = confidence_threshold;
    pd_nms_thresh = nms_threshold;
}

void NNGestureDetection::begin(void)
{
    // Palm Detection Configuration
    if (_p_mmf_context == NULL) {
        _p_mmf_context = mm_module_open(&vipnn_module);
    }
    if (_p_mmf_context == NULL) {
        printf("\r\n[ERROR] NNPalmDetection init failed\n");
        return;
    }
    if ((palm_nn_roi.img.width == 0) || (palm_nn_roi.img.height == 0)) {
        printf("\r\n[ERROR] NNPalmDetection video not configured\n");
        return;
    }

    if (_nntask != GESTURE_DETECTION) {
        if (ARDUINO_LOAD_MODEL == 0x02) {
            printf("\r\n[INFO] Models loaded using SD Card\n");
        } else {
            while (1) {
                printf("\r\n[ERROR] Invalid NN task selected! Please check modelSelect() again\n");
                delay(5000);
            }
        }
    }

    if (ARDUINO_LOAD_MODEL == 0x02) {
        vfs_init(NULL);    // init filesystem
        vfs_user_register("sd", VFS_FATFS, VFS_INF_SD);
        vipnn_control(_p_mmf_context->priv, CMD_VIPNN_SET_MODEL, (int)&palmdet_from_sd);
    } else {
        vipnn_control(_p_mmf_context->priv, CMD_VIPNN_SET_MODEL, (int)&palm_detection_fwfs);
    }
    vipnn_control(_p_mmf_context->priv, CMD_VIPNN_SET_IN_PARAMS, (int)&palm_nn_roi);
    mm_module_ctrl(_p_mmf_context, CMD_VIPNN_SET_OUTPUT, 1);
    mm_module_ctrl(_p_mmf_context, CMD_VIPNN_SET_RES_SIZE, sizeof(palmdetect_res_t));    // result size
    mm_module_ctrl(_p_mmf_context, CMD_VIPNN_SET_RES_MAX_CNT, MAX_HAND_DETECT_NUM);      // result max count
    mm_module_ctrl(_p_mmf_context, MM_CMD_SET_DATAGROUP, MM_GROUP_START);
    vipnn_control(_p_mmf_context->priv, CMD_VIPNN_SET_CONFIDENCE_THRES, (int)&pd_confidence_thresh);
    vipnn_control(_p_mmf_context->priv, CMD_VIPNN_SET_NMS_THRES, (int)&pd_nms_thresh);
    mm_module_ctrl(_p_mmf_context, CMD_VIPNN_SET_OUTPUT, 1);    // enable module output
    mm_module_ctrl(_p_mmf_context, MM_CMD_SET_QUEUE_LEN, 1);
    mm_module_ctrl(_p_mmf_context, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
    vipnn_control(_p_mmf_context->priv, CMD_VIPNN_APPLY, 0);

    // Hand Landmark
    if (handlandmark_ctx == NULL) {
        handlandmark_ctx = mm_module_open(&vipnn_module);
    }
    if (handlandmark_ctx == NULL) {
        printf("\r\n[ERROR] NNHandLandmark init failed\n");
        return;
    }
    if (palm_hand_siso_ctx == NULL) {
        palm_hand_siso_ctx = (void *)sisoCreate();
    }
    if (palm_hand_siso_ctx == NULL) {
        printf("\r\n[ERROR] HandLandmark SISO init failed\n");
        return;
    }

    // Hand Landmark configuration
    if (ARDUINO_LOAD_MODEL == 0x02) {
        vfs_init(NULL);    // init filesystem
        vfs_user_register("sd", VFS_FATFS, VFS_INF_SD);
        vipnn_control(handlandmark_ctx->priv, CMD_VIPNN_SET_MODEL, (int)&handlandmark_from_sd);
    } else {
        vipnn_control(handlandmark_ctx->priv, CMD_VIPNN_SET_MODEL, (int)&hand_landmark_fwfs);
    }

    vipnn_control(handlandmark_ctx->priv, CMD_VIPNN_SET_CASCADE, 2);    // this module is cascade mode
    vipnn_control(handlandmark_ctx->priv, CMD_VIPNN_SET_OUTPUT, 1);     // output
    mm_module_ctrl(handlandmark_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&handlandmark_nn_roi);
    mm_module_ctrl(handlandmark_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(handland_res_t));    // result size
    mm_module_ctrl(handlandmark_ctx, CMD_VIPNN_SET_RES_MAX_CNT, MAX_HAND_DETECT_NUM);    // result max count

    mm_module_ctrl(handlandmark_ctx, MM_CMD_SET_DATAGROUP, MM_GROUP_END);
    mm_module_ctrl(handlandmark_ctx, MM_CMD_SET_QUEUE_LEN, 1);
    mm_module_ctrl(handlandmark_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
    mm_module_ctrl(handlandmark_ctx, CMD_VIPNN_SET_DISPPOST, (int)drawHandObject);
    vipnn_control(handlandmark_ctx->priv, CMD_VIPNN_APPLY, 0);

    // SISO link palm detection to HandLandmark
    sisoRegIn(palm_hand_siso_ctx, _p_mmf_context);
    sisoRegOut(palm_hand_siso_ctx, handlandmark_ctx);
    sisoStart(palm_hand_siso_ctx);
}

void NNGestureDetection::end(void)
{
    if (palm_hand_siso_ctx == NULL) {
        return;
    }
    sisoDestroy(palm_hand_siso_ctx);
    palm_hand_siso_ctx = NULL;

    if (_p_mmf_context == NULL) {
        return;
    }

    if (mm_module_close(_p_mmf_context) == NULL) {
        _p_mmf_context = NULL;
    } else {
        printf("\r\n[ERROR] NNPalmDetection deinit failed\n");
    }

    if (handlandmark_ctx == NULL) {
        return;
    }
    if (mm_module_close(handlandmark_ctx) == NULL) {
        handlandmark_ctx = NULL;
    } else {
        printf("\r\n[ERROR] NNHandLandmark deinit failed\n");
    }
}

void NNGestureDetection::setResultCallback(void (*gd_callback)(std::vector<GestureDetectionResult>))
{
    GD_user_CB = gd_callback;
}

uint16_t NNGestureDetection::getResultCount(void)
{
    uint16_t gesturedetect_res_count = hand_result_vector.size();
    if (gesturedetect_res_count > 128) {
        gesturedetect_res_count = 128;
    }
    return gesturedetect_res_count;
}

GestureDetectionResult NNGestureDetection::getResult(uint16_t index)
{
    if (index >= hand_result_vector.size()) {
        return GestureDetectionResult();
    }
    return hand_result_vector[index];
}

std::vector<GestureDetectionResult> NNGestureDetection::getResult(void)
{
    return hand_result_vector;
}

void NNGestureDetection::drawHandObject(void *p, void *img_param)
{
    if (!p || !img_param) {
        return;
    }

    vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
    handland_res_t *res = (handland_res_t *)&out->res[0];
    nn_data_param_t *im = (nn_data_param_t *)img_param;

    int im_h = RTSP_HEIGHT;
    int im_w = RTSP_WIDTH;

    // printf("im->width = %d, im->height = %d\r\n", im->img.width, im->img.height);
    im->img.width = im->img.height = handlandmark_nn_roi.img.width;

    float ratio_w = (float)im_w / (float)im->img.width;
    float ratio_h = (float)im_h / (float)im->img.height;
    float ratio = ratio_h < ratio_w ? ratio_h : ratio_w;

    roi_delta_qp_set_param(RTSP_CHANNEL, 0, 0, RTSP_WIDTH, RTSP_HEIGHT, ROI_DELTA_QP_MAX);
    OSD.createBitmap(RTSP_CHANNEL, HAND_JOINT_LAYER);
    OSD.createBitmap(RTSP_CHANNEL, HAND_LINK_LAYER);

    if (res->handedness != HANDEDNESS_NOTFOUND) {
        // printf("theta:%f w:%d h:%d\r\n", res->theta, res->w, res->h);
        if (fixLandmark(res) < 0) {
            return;
        }

        // First, draw the points for the joints.
        for (int i = 0; i < HAND_LANDMARK_NUM; i++) {
            // printf("res->landmark3d.pos[i].x = %f, res->landmark3d.pos[i].y = %f, res->landmark3d.pos[i].z = %f\r\n", res->landmark3d.pos[i].x, res->landmark3d.pos[i].y, res->landmark3d.pos[i].z);
            int xr = res->landmark3d.pos[i].x * ratio + (RTSP_WIDTH - RTSP_HEIGHT) / 2;
            int yr = res->landmark3d.pos[i].y * ratio;
            OSD.drawPoint(RTSP_CHANNEL, xr, yr, 12, COLOR_RED, HAND_JOINT_LAYER);
            // res->landmark3d.pos[i].z = roundf(q2f(&llm[(i*3+2)*datasize], llm_fmt.format));//Not needed to draw OSD
            // printf("llm %d(%f,%f,%f)\r\n", i, hand_landmark_res->landmark3d.pos[i].x, hand_landmark_res->landmark3d.pos[i].y, hand_landmark_res->landmark3d.pos[i].z);
        }

        /*
        #        8   12  16  20
        #        |   |   |   |
        #        7   11  15  19
        #    4   |   |   |   |
        #    |   6   10  14  18
        #    3   |   |   |   |
        #    |   5---9---13--17
        #    2    \         /
        #     \    \       /
        #      1    \     /
        #       \    \   /
        #        ------0-
        connections = [
            (0, 1), (1, 2), (2, 3), (3, 4),
            (5, 6), (6, 7), (7, 8),
            (9, 10), (10, 11), (11, 12),
            (13, 14), (14, 15), (15, 16),
            (17, 18), (18, 19), (19, 20),
            (0, 5), (5, 9), (9, 13), (13, 17), (0, 17), (2,5)
        */
        int connections[22][2] = {
            {0,  1 },
            {1,  2 },
            {2,  3 },
            {3,  4 },
            {5,  6 },
            {6,  7 },
            {7,  8 },
            {9,  10},
            {10, 11},
            {11, 12},
            {13, 14},
            {14, 15},
            {15, 16},
            {17, 18},
            {18, 19},
            {19, 20},
            {0,  5 },
            {5,  9 },
            {9,  13},
            {13, 17},
            {0,  17},
            {2,  5 }
        };

        // Draw the connections of the joints
        for (int i = 0; i < 22; i++) {
            int idx_start = connections[i][0];
            int idx_end = connections[i][1];
            int start_x = res->landmark3d.pos[idx_start].x * ratio + (RTSP_WIDTH - RTSP_HEIGHT) / 2;
            int start_y = res->landmark3d.pos[idx_start].y * ratio;
            int end_x = res->landmark3d.pos[idx_end].x * ratio + (RTSP_WIDTH - RTSP_HEIGHT) / 2;
            int end_y = res->landmark3d.pos[idx_end].y * ratio;
            // printf("start_x = %d, start_y = %d, end_x = %d, end_y = %d\r\n", start_x, start_y, end_x, end_y);
            OSD.drawLine(RTSP_CHANNEL, start_x, start_y, end_x, end_y, 8, COLOR_GREEN, HAND_LINK_LAYER);
        }
        if (osd_cleanup_timer) {
            xTimerReset(osd_cleanup_timer, 10);
        }
    }

    OSD.update(RTSP_CHANNEL, 0, HAND_JOINT_LAYER);
    OSD.update(RTSP_CHANNEL, 1, HAND_LINK_LAYER);
}

/*
Correcting coordinates
After performing a series of rotations, scaling, and translations, the coordinates need to be corrected back.
For example, if you previously rotated the hand to point the middle finger upwards, you need to rotate it back here; otherwise, the drawn hand will always have the middle finger pointing up.
*/
int NNGestureDetection::fixLandmark(handland_res_t *lm)
{
    float cx = (float)handlandmark_nn_roi.img.width / 2.0;
    float cy = (float)handlandmark_nn_roi.img.height / 2.0;
    float cos_theta = cosf(lm->theta);
    float sin_theta = sinf(lm->theta);
    float scale = lm->w / (float)(handlandmark_nn_roi.img.width / 2.0) / lm->ratio;
    for (int i = 0; i < HAND_LANDMARK_NUM; i++) {
        // printf("before:%d x = %f, y = %f\r\n", i, lm->landmark3d.pos[i].x, lm->landmark3d.pos[i].y);
        // Place your hand at the edge of the screen. Sometimes, the landmark will go out of the frame. Let's correct it here first.
        LIMIT(lm->landmark3d.pos[i].x, 0, handlandmark_nn_roi.img.width);
        LIMIT(lm->landmark3d.pos[i].y, 0, handlandmark_nn_roi.img.height);
        float dx = lm->landmark3d.pos[i].x - cx;
        float dy = lm->landmark3d.pos[i].y - cy;
        float x = dx * cos_theta - dy * sin_theta + cx;
        float y = dx * sin_theta + dy * cos_theta + cy;
        if (x < 0 || y < 0) {
            // Place your hand at the edge of the screen. After calibration, it might extend far beyond the screen, so let's not draw this for now.
            printf("error! x = %f, y = %f\r\n", x, y);
            return -1;
        }
        x = x * scale;
        y = y * scale;
        lm->landmark3d.pos[i].x = x + lm->offset_x;
        lm->landmark3d.pos[i].y = y + lm->offset_y;
        // printf("after:%d x = %f, y = %f\r\n", i, lm->landmark3d.pos[i].x, lm->landmark3d.pos[i].y);

        // Here are some additional adjustments to make the skeleton more accurate when the hand is in the upper corner of the screen.
        if (lm->offset_x < 20) {
            lm->landmark3d.pos[i].x = lm->landmark3d.pos[i].x - (20 - lm->offset_x);
            LIMIT(lm->landmark3d.pos[i].x, 0, handlandmark_nn_roi.img.width);
        }
        if (lm->offset_y < 20) {
            lm->landmark3d.pos[i].y = lm->landmark3d.pos[i].y - (20 - lm->offset_y);
            LIMIT(lm->landmark3d.pos[i].y, 0, handlandmark_nn_roi.img.height);
        }
    }
    // printf("lm->offset_x = %d, lm->offset_y = %d\r\n", lm->offset_x, lm->offset_y);
    return 0;
}

void NNGestureDetection::handCleanupCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    OSD.createBitmap(RTSP_CHANNEL, HAND_JOINT_LAYER);
    OSD.createBitmap(RTSP_CHANNEL, HAND_LINK_LAYER);
    OSD.update(RTSP_CHANNEL, 0, HAND_JOINT_LAYER);
    OSD.update(RTSP_CHANNEL, 1, HAND_LINK_LAYER);
}

void NNGestureDetection::drawHandRegion(void)
{
    // draw hand detect region
    OSD.createBitmap(RTSP_CHANNEL, HAND_DETECT_REGION_LAYER);
    OSD.drawRect(RTSP_CHANNEL, (RTSP_WIDTH - RTSP_HEIGHT) / 2, 5, RTSP_WIDTH - (RTSP_WIDTH - RTSP_HEIGHT) / 2, RTSP_HEIGHT - 5, 3,
                 COLOR_BLUE, HAND_DETECT_REGION_LAYER);
    OSD.update(RTSP_CHANNEL, 1, HAND_DETECT_REGION_LAYER);
    osd_cleanup_timer = xTimerCreate("OSD clean timer", 1000 / portTICK_PERIOD_MS, pdTRUE, NULL, handCleanupCallback);
}
