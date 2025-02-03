#ifndef __NNMODELSELECTION_H__
#define __NNMODELSELECTION_H__

#include "VideoStream.h"

#define NA_MODEL              0x00
#define DEFAULT_YOLOV3TINY    0x01
#define DEFAULT_YOLOV4TINY    0x02
#define DEFAULT_YOLOV7TINY    0x03
#define DEFAULT_SCRFD         0x04
#define DEFAULT_MOBILEFACENET 0x05
#define DEFAULT_YAMNET        0x06
#define DEFAULT_IMGCLASS      0x07
#define DEFAULT_PALMDETECT    0x08
#define DEFAULT_HANDLANDMARK  0x09

#if ARDUINO_LOAD_MODEL == 0x02
#define CUSTOMIZED_YOLOV3TINY    SD_YOLOV3TINY
#define CUSTOMIZED_YOLOV4TINY    SD_YOLOV4TINY
#define CUSTOMIZED_YOLOV7TINY    SD_YOLOV7TINY
#define CUSTOMIZED_SCRFD         SD_SCRFD
#define CUSTOMIZED_MOBILEFACENET SD_MBFACENET
#define CUSTOMIZED_YAMNET        SD_YAMNET
#define CUSTOMIZED_IMGCLASS      SD_IMGCLASSIFICATION
#define CUSTOMIZED_PALMDETECT    SD_PALMDETECT
#define CUSTOMIZED_HANDLANDMARK  SD_HANDLANDMARK
#else
#define CUSTOMIZED_YOLOV3TINY    0x11
#define CUSTOMIZED_YOLOV4TINY    0x12
#define CUSTOMIZED_YOLOV7TINY    0x13
#define CUSTOMIZED_SCRFD         0x14
#define CUSTOMIZED_MOBILEFACENET 0x15
#define CUSTOMIZED_YAMNET        0x16
#define CUSTOMIZED_IMGCLASS      0x17
#define CUSTOMIZED_PALMDETECT    0x18
#define CUSTOMIZED_HANDLANDMARK  0x19
#endif

#define OBJECT_DETECTION     0x21
#define FACE_DETECTION       0x22
#define FACE_RECOGNITION     0x23
#define AUDIO_CLASSIFICATION 0x24
#define IMAGE_CLASSIFICATION 0x25
#define GESTURE_DETECTION    0X26

#define SD_YOLOV3TINY        0x31
#define SD_YOLOV4TINY        0x32
#define SD_YOLOV7TINY        0x33
#define SD_SCRFD             0x34
#define SD_MBFACENET         0x35
#define SD_YAMNET            0x36
#define SD_IMGCLASSIFICATION 0x37
#define SD_PALMDETECT        0x38
#define SD_HANDLANDMARK      0x39

class NNModelSelection: public MMFModule {
public:
    void modelSelect(unsigned char nntask);
    void modelSelect(unsigned char nntask, unsigned char objdetmodel, unsigned char facedetmodel, unsigned char facerecogmodel, unsigned char audclassmodel);
    void modelSelect(unsigned char nntask, unsigned char objdetmodel, unsigned char facedetmodel, unsigned char facerecogmodel);
    void modelSelect(unsigned char nntask, unsigned char objdetmodel, unsigned char facedetmodel, unsigned char facerecogmodel, unsigned char audclassmodel, unsigned char imgclassmodel);
    void modelSelect(unsigned char nntask, unsigned char objdetmodel, unsigned char facedetmodel, unsigned char facerecogmodel, unsigned char audclassmodel, unsigned char imgclassmodel, unsigned char palmdetmodel, unsigned char handlandmarkmodel);

protected:
    unsigned char _yolomodel;
    unsigned char _scrfdmodel;
    unsigned char _mfnmodel;
    unsigned char _yamnetmodel;
    unsigned char _imgclassmodel;
    unsigned char _palmdetmodel;
    unsigned char _handlandmarkmodel;
    unsigned char _nntask;
};
#endif
