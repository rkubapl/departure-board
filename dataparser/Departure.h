#pragma once
#include <string>
#include <cstdint>

class Departure {
public:
    std::string lineName;
    std::string destinationName;
    uint8_t upcomingStopCount;
    uint32_t routePatternOffset;
    uint32_t arrivalTimePatternOffset;
    uint64_t departureUnixTime;

    uint64_t getMinutesToDeparture(uint64_t unixTime);
};
