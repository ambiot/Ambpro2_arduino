#ifndef __MOTIONDETECTION_H__
#define __MOTIONDETECTION_H__

#include "md_api.h"
#include "VideoStream.h"

#undef min
#undef max
#include <vector>

// Class not used currently, require raw MD result
#if 0
#define MDMAXREGIONS (32*32/2)

class MotionDetectionRegion {
    friend class MotionDetectionPostProcess;
    public:
        uint16_t size(void);
        float xMin(void);
        float xMax(void);
        float yMin(void);
        float yMax(void);

    private:
        uint16_t blockCount = 0;
        uint8_t xmin = 255;
        uint8_t xmax = 0;
        uint8_t ymin = 255;
        uint8_t ymax = 0;
        uint8_t rows = 0;
        uint8_t cols = 0;
};

class MotionDetectionPostProcess {
    public:
        uint16_t labelAdjacentRegions(char* mdResult, uint8_t rows, uint8_t cols);
        uint16_t getRegionCount(void);
        MotionDetectionRegion getRegion(uint16_t index);
        std::vector<MotionDetectionRegion> getRegion(void);

    private:
        uint16_t regionCount;
        uint8_t regionGroup[MDMAXREGIONS] = {0};
        uint8_t groupCount = 0;
        uint8_t groupMap[MDMAXREGIONS] = {0};
        std::vector<MotionDetectionRegion> region_vector;
};
#endif

class MotionDetection:public MMFModule {
    public:
        MotionDetection(uint8_t row = 18, uint8_t col = 32);
        ~MotionDetection(void);

        void configResolution(uint8_t row = 18, uint8_t col = 32);
        void configVideo(VideoSetting& config);
        void begin(void);
        void end(void);
        void setTriggerBlockCount(uint16_t count);
        void setDetectionMask(char * mask);

        md_result_t* getResult(void);
        void setResultCallback(void (*md_callback)(md_result_t*));
        uint8_t rows(void);
        uint8_t cols(void);

    private:
        md_param_t md_param = {0};
        motion_detect_threshold_t md_thr = {2,3};
        md_result_t md_result = {0};
        void (*md_ResultCB)(md_result_t*) = NULL;
        uint16_t trigCount = 0;
};

#endif
