#include <Arduino.h>
#include "NNModelSelection.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mmf2_module.h"
#include "model_yolo.h"
#include "model_scrfd.h"
#include "model_mobilefacenet.h"
#include "model_yamnet.h"
#include "model_classification.h"

extern void yolov3_set_network_init_info(void *m);
extern void yolov4_set_network_init_info(void *m);
extern void yolov7_set_network_init_info(void *m);
extern int yolo_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int yolo_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);
extern void yolo_set_confidence_thresh(void *confidence_thresh);
extern void yolo_set_nms_thresh(void *nms_thresh);

extern int yamnet_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int yamnet_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);

extern void scrfd_set_network_init_info(void *m);
extern int scrfd_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int scrfd_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);
extern void scrfd_set_confidence_thresh(void *confidence_thresh);
extern void scrfd_set_nms_thresh(void *nms_thresh);

extern int mbfacenet_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int mbfacenet_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);
extern void mbfacenet_input_setup(void *last_res, int i, nn_data_param_t *input_param);

extern int classification_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int classification_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);

extern void palm_detection_set_network_init_info(void *m);
extern int palm_detection_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int palm_detection_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);
extern void palm_detection_set_confidence_thresh(void *confidence_thresh);
extern void palm_detection_set_nms_thresh(void *nms_thresh);

extern int hand_landmark_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param);
extern int hand_landmark_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res);
extern void hand_landmark_set_confidence_thresh(void *confidence_thresh);
extern void hand_landmark_set_nms_thresh(void *nms_thresh);
extern void hand_landmark_input_setup(void *last_res, int i, nn_data_param_t *input_param);

#ifdef __cplusplus
}
#endif

static void *yolov3_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/yolov3_tiny.nb";
}

static void *yolov4_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/yolov4_tiny.nb";
}

static void *yolov7_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/yolov7_tiny.nb";
}

static void *scrfd_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/scrfd.nb";
}

static void *mobilefacenet_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/mobilefacenet.nb";
}

static void *yamnet_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/yamnet.nb";
}

static void *classification_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/imgclassification.nb";
}

static void *palmdet_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/palm_detection.nb";
}

static void *handlandmark_get_SD_filename(void)
{
    /* set filename of customized model */
    return (void *)"sd:/NN_MDL/hand_landmark.nb";
}

nnmodel_t yolov3_tiny_from_sd = {
    .nb = yolov3_get_SD_filename,
    .preprocess = yolo_preprocess,
    .postprocess = yolo_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_init_info = yolov3_set_network_init_info,
    .set_confidence_thresh = yolo_set_confidence_thresh,
    .set_nms_thresh = yolo_set_nms_thresh,

    .name = "YOLOv3t_SD"};

nnmodel_t yolov4_tiny_from_sd = {
    .nb = yolov4_get_SD_filename,
    .preprocess = yolo_preprocess,
    .postprocess = yolo_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_init_info = yolov4_set_network_init_info,
    .set_confidence_thresh = yolo_set_confidence_thresh,
    .set_nms_thresh = yolo_set_nms_thresh,

    .name = "YOLOv4t_SD"};

nnmodel_t yolov7_tiny_from_sd = {
    .nb = yolov7_get_SD_filename,
    .preprocess = yolo_preprocess,
    .postprocess = yolo_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_init_info = yolov7_set_network_init_info,
    .set_confidence_thresh = yolo_set_confidence_thresh,
    .set_nms_thresh = yolo_set_nms_thresh,

    .name = "YOLOv7t_SD"};

nnmodel_t scrfd_fwfs_from_sd = {
    .nb = scrfd_get_SD_filename,
    .preprocess = scrfd_preprocess,
    .postprocess = scrfd_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_init_info = scrfd_set_network_init_info,
    .set_confidence_thresh = scrfd_set_confidence_thresh,
    .set_nms_thresh = scrfd_set_nms_thresh,

    .name = "SCRFD_SD"};

nnmodel_t mbfacenet_fwfs_from_sd = {
    .nb = mobilefacenet_get_SD_filename,
    .preprocess = mbfacenet_preprocess,
    .postprocess = mbfacenet_postprocess,
    .model_src = MODEL_SRC_FILE,
    .cas_in_setup = mbfacenet_input_setup,

    .name = "MBFACENET_SD"};

nnmodel_t yamnet_from_sd = {
    .nb = yamnet_get_SD_filename,
    .preprocess = yamnet_preprocess,
    .postprocess = yamnet_postprocess,

    .model_src = MODEL_SRC_FILE,

    .name = "YAMNET_SD"};

nnmodel_t img_classification_from_sd = {
    .nb = classification_get_SD_filename,
    .preprocess = classification_preprocess,
    .postprocess = classification_postprocess,
    .model_src = MODEL_SRC_FILE,

    .name = "IMGCLASSIFICATION_SD"};

nnmodel_t palmdet_from_sd = {
    .nb = palmdet_get_SD_filename,
    .preprocess = palm_detection_preprocess,
    .postprocess = palm_detection_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_init_info = palm_detection_set_network_init_info,
    .set_confidence_thresh = palm_detection_set_confidence_thresh,
    .set_nms_thresh = palm_detection_set_nms_thresh,

    .name = "PALM_DETECTION_SD"};

nnmodel_t handlandmark_from_sd = {
    .nb = handlandmark_get_SD_filename,
    .preprocess = hand_landmark_preprocess,
    .postprocess = hand_landmark_postprocess,
    .model_src = MODEL_SRC_FILE,
    .set_confidence_thresh = hand_landmark_set_confidence_thresh,
    .set_nms_thresh = hand_landmark_set_nms_thresh,
    .cas_in_setup = hand_landmark_input_setup,

    .name = "HAND_LANDMARK_SD"};
