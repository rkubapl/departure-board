#include "TimeManager.h"

#ifdef ESP_PLATFORM
#include <sys/time.h>

void TimeManager::setTime(time_t newTime) {
    if (newTime != -1) {
        timeval tv = {};
        tv.tv_sec = newTime;
        tv.tv_usec = 0;
        settimeofday(&tv, nullptr);
    }
}

time_t TimeManager::getTime() {
    return time(nullptr);
}

#else

static time_t timeOffset = 0;

void TimeManager::setTime(time_t newTime) {
    if (newTime != -1) {
        time_t systemTime = time(nullptr);
        timeOffset = newTime - systemTime;
    }
}

time_t TimeManager::getTime() {
    return time(nullptr) + timeOffset;
}

#endif
