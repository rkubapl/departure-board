#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include "Departure.h"
#include "IDataSource.h"

typedef std::pair<std::string, uint32_t> selectStop;
typedef std::pair<std::string, int> ArriveStop;

class TimetableLoader {
  IDataSource &source;

  uint32_t startDate;
  uint16_t daysAmount;
  uint32_t directoryOffset;
  uint32_t stringTableOffset;
  uint32_t calendarOffset;

  std::vector<selectStop> selectStops;
  std::vector<selectStop> readStopsToSelect();

  std::vector<std::string> readRoutePattern(uint8_t count, uint32_t offset);
  std::vector<uint16_t> readArrivalTimePattern(uint8_t count, uint32_t offset);

    uint16_t readU16LE(const uint8_t *buf);
    uint32_t readU32LE(const uint8_t *buf);

public:
  TimetableLoader(IDataSource &src) : source(src) {}
  void load();
  std::vector<bool> getServicesForDay(uint32_t unixDay);
  const std::vector<selectStop> &getStopsToSelect() const;
  std::queue<Departure> getNextDepartures(size_t stopIndex, uint64_t unixTime);
  std::vector<ArriveStop> getDetailedInfo(uint8_t count, uint64_t departureTime,
                                          uint32_t routePatternOffset,
                                          uint32_t arrivalTimePatternOffset);

  std::string readString(uint32_t offset) const;
    //TODO: dodac friend z Departure
};