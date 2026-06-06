
#include "Departure.h"

uint64_t Departure::getMinutesToDeparture(uint64_t unixTime) const {
    int64_t diffSec = static_cast<int64_t>(departureUnixTime) - static_cast<int64_t>(unixTime);
    return diffSec / 60;
}

std::string Departure::getLineName() const {
    return lineName;
}

std::string Departure::getDestinationName() const {
    return destinationName;
}

uint8_t Departure::getUpcomingStopCount() const {
    return upcomingStopCount;
}

uint32_t Departure::getRoutePatternOffset() const {
    return routePatternOffset;
}

uint32_t Departure::getArrivalTimePatternOffset() const {
    return arrivalTimePatternOffset;
}

uint64_t Departure::getDepartureUnixTime() const {
    return departureUnixTime;
}
