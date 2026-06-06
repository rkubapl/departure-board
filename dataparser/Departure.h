#pragma once
#include <string>
#include <cstdint>

class Departure {
    std::string lineName;
    std::string destinationName;
    uint8_t upcomingStopCount;
    uint32_t routePatternOffset;
    uint32_t arrivalTimePatternOffset;
    uint64_t departureUnixTime;
public:
    std::string getLineName() const;
    std::string getDestinationName() const;
    uint8_t getUpcomingStopCount() const;
    uint32_t getRoutePatternOffset() const;
    uint32_t getArrivalTimePatternOffset() const;
    uint64_t getDepartureUnixTime() const;

    uint64_t getMinutesToDeparture(uint64_t unixTime) const;

    friend class TimetableLoader;
};
