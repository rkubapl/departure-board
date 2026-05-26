
#include "Departure.h"

uint64_t Departure::getMinutesToDeparture(uint64_t unixTime) {
    int64_t diffSec = static_cast<int64_t>(departureUnixTime) - static_cast<int64_t>(unixTime);
    return diffSec / 60;
}
