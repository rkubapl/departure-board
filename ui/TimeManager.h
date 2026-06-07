#pragma once

#include <ctime>
#include <cstdint>

class TimeManager {
public:
    static void setTime(time_t newTime);
    static time_t getTime();
};
