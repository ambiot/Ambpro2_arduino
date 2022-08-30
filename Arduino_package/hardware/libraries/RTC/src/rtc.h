#ifndef __RTC_H__
#define __RTC_H__

#include <Arduino.h>

#include "rtc_api.h"
#include "timer_api.h"

class AMB_RTC {
    public:
        AMB_RTC(void);
        ~AMB_RTC(void);
        void Init(void);
        void DeInit(void);
        void Write(long long t);
        long long Read(void);
        void Wait(int s);
        long long SetEpoch(int year, int month, int day, int hour, int min, int sec);
        void EnableAlarm(int day, int hour, int min, int sec, void (*rtc_handler)(void));
        void DisableAlarm(void);

    private:

};
#endif
