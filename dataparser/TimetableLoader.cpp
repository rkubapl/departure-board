#include "TimetableLoader.h"
#include <algorithm>
#include <stdexcept>
#include <format>
#include <cstdio>
#include <ctime>
#include <optional>

//note: what to do with exceptions? should they stay or remove them to reduce memory usage

inline uint16_t TimetableLoader::readU16LE(const uint8_t *buf) {
  return static_cast<uint16_t>(buf[0] | (buf[1] << 8));
}

inline uint32_t TimetableLoader::readU32LE(const uint8_t *buf) {
  return static_cast<uint32_t>(buf[0]) | (static_cast<uint32_t>(buf[1]) << 8) |
         (static_cast<uint32_t>(buf[2]) << 16) |
         (static_cast<uint32_t>(buf[3]) << 24);
}

class InvalidFileFormat : public std::runtime_error {
public:
  explicit InvalidFileFormat(const std::string &message)
      : std::runtime_error(message) {}
};

void TimetableLoader::load() {
  if (!source.isOpen())
    throw std::runtime_error("Failed to open the file");

  source.seek(0);

  uint8_t header[24];
  if (source.read(header, 24) != 24)
    throw std::runtime_error("Failed to read file: header");

  if (header[0] != 'P' || header[1] != 'K' || header[2] != 'P' || header[3] != 'K') {
    throw InvalidFileFormat("Invalid magic number.");
  }

  uint16_t versionNumber = readU16LE(header + 4);
  if (versionNumber != 1)
    throw InvalidFileFormat("Invalid version.");

  startDate = readU32LE(header + 6);
  daysAmount = readU16LE(header + 10);
  directoryOffset = readU32LE(header + 12);
  stringTableOffset = readU32LE(header + 16);
  calendarOffset = readU32LE(header + 20);
  selectStops = readStopsToSelect();
}

std::vector<bool> TimetableLoader::getServicesForDay(uint32_t unixDay) {
  if (unixDay < startDate || unixDay >= startDate + daysAmount) {
    throw std::invalid_argument("Day out of range");
  }

  source.seek(calendarOffset);

  uint16_t totalCalendars;
  if (source.read(reinterpret_cast<uint8_t *>(&totalCalendars), 2) != 2)
    throw std::runtime_error(
        "Failed to read file: calendars -> total calendars");

  uint32_t dayIndex = unixDay - startDate;
  uint32_t byteIndex = dayIndex / 8;
  uint32_t bitIndex = dayIndex % 8;
  uint32_t B = (daysAmount + 7) / 8;

  std::vector<bool> services;
  services.reserve(totalCalendars);

  for (uint16_t k = 0; k < totalCalendars; ++k) {
    uint32_t offset = calendarOffset + 2 + k * B + byteIndex;
    if (!source.seek(offset)) {
      throw std::runtime_error("Failed to seek to calendar entry byte");
    }

    uint8_t byteVal;
    if (source.read(&byteVal, 1) != 1) {
      throw std::runtime_error("Failed to read calendar entry byte");
    }

    bool isActive = (byteVal >> bitIndex) & 1;
    services.push_back(isActive);
  }

  return services;
}

std::string TimetableLoader::readString(uint32_t offset) const {
  if (!source.seek(offset)) {
    throw std::runtime_error("Failed to seek to string offset");
  }

  std::string out;
  out.reserve(32);

  for (size_t i = 0; i < 1000; ++i) {
    uint8_t ch = 0;
    if (source.read(&ch, 1) != 1) {
      throw std::runtime_error("EOF while reading null-terminated string");}

    if (ch == '\0') {
      return out;
    }

    out.push_back(static_cast<char>(ch));
  }

  throw std::runtime_error("Null terminator not found (string too long or corrupted data)");
}

std::vector<selectStop> TimetableLoader::readStopsToSelect() {
  if (!source.seek(directoryOffset)) {
    throw std::runtime_error("Failed to seek to directory");
  }

  uint16_t count;
  if (source.read(reinterpret_cast<uint8_t *>(&count), 2) != 2) {
    throw std::runtime_error("Failed to read file: directory -> stop_count");
  }

  std::vector<selectStop> stops;
  stops.reserve(count);

  for (uint16_t i = 0; i < count; i++) {
    uint32_t currentEntryOffset = directoryOffset + 2 + i * 8;
    if (!source.seek(currentEntryOffset)) {
      throw std::runtime_error("Failed to seek to directory entry");
    }

    uint32_t nameOffset;
    uint32_t scheduleOffset;

    if (source.read(reinterpret_cast<uint8_t *>(&nameOffset), 4) != 4) {
      throw std::runtime_error(
          "Failed to read file: directory entry -> name offset");
    }
    if (source.read(reinterpret_cast<uint8_t *>(&scheduleOffset), 4) != 4) {
      throw std::runtime_error(
          "Failed to read file: directory entry -> schedule offset");
    }

    stops.push_back({readString(nameOffset), scheduleOffset});
  }

  return stops;
}

std::optional<Departure> TimetableLoader::getDeparture(uint32_t offset, std::vector<bool> calendar, uint64_t unixDay) {
  if (!source.seek(offset)) {
    throw std::runtime_error("Failed to seek to trip info");
  }

  uint8_t tripData[21];
  if (source.read(tripData, 21) != 21) {
    throw std::runtime_error("Failed to read trip data");
  }

  uint32_t tripCalendarID = readU16LE(tripData + 11);

  if (tripCalendarID >= calendar.size()) {
    throw std::runtime_error("Invalid calendar index calculated: " + std::to_string(tripCalendarID));
  }

  if (!calendar[tripCalendarID]) {
    return std::nullopt;
  }

  Departure dep;
  dep.lineName = readString(readU32LE(tripData));
  dep.destinationName = readString(readU32LE(tripData + 4));
  dep.upcomingStopCount = tripData[10];
  dep.routePatternOffset = readU32LE(tripData + 13);
  dep.arrivalTimePatternOffset = readU32LE(tripData + 17);
  dep.departureUnixTime = unixDay * 86400 + readU16LE(tripData + 8)*60;

  return dep;
}

const std::vector<selectStop> &TimetableLoader::getStopsToSelect() const {
  return selectStops;
}

std::list<Departure> TimetableLoader::getNextDepartures(size_t stopIndex, uint64_t unixTime) {
  if (stopIndex >= selectStops.size()) {
    throw std::out_of_range("Invalid stop index");
  }

  uint64_t unixTimeUntil = unixTime + 86400;

  auto t = static_cast<time_t>(unixTime);
  tm tm_info = *localtime(&t);

  uint32_t unixDay = unixTime / 86400;
  uint16_t currentMinutes = tm_info.tm_hour * 60 + tm_info.tm_min;

  uint32_t scheduleOffset = selectStops[stopIndex].second;
  if (!source.seek(scheduleOffset)) {
    throw std::runtime_error("Failed to seek to schedule");
  }

  uint16_t tripCount;
  if (source.read(reinterpret_cast<uint8_t *>(&tripCount), 2) != 2) {
    throw std::runtime_error("Failed to read trip count");
  }

  if (tripCount == 0) return {};

  std::vector<Departure> departures;
  departures.reserve(30);

  for (int dayOffset = -1; dayOffset < 3; ++dayOffset) { //TODO: get only departures for next 24h
    if (dayOffset > 0 && departures.size() >= 20)
      break;

    uint32_t currentUnixDay = unixDay + dayOffset;
    if (currentUnixDay < startDate || currentUnixDay >= startDate + daysAmount)
      continue;

    std::vector<bool> activeCalendar = getServicesForDay(currentUnixDay);

    int targetDepTime = currentMinutes - (dayOffset * 1440);
    int startIndex = 0;

    if (targetDepTime > 0) {
      int left = 0;
      int right = tripCount - 1;
      startIndex = tripCount;

      while (left <= right) {
        int mid = left + (right - left) / 2;
        uint32_t tripOffset = scheduleOffset + 2 + mid * 21;

        if (!source.seek(tripOffset + 8)) {
          throw std::runtime_error("Failed to seek to departure time");
        }

        uint16_t departureTime;
        if (source.read(reinterpret_cast<uint8_t *>(&departureTime), 2) != 2) {
          throw std::runtime_error("Failed to read departure time");
        }

        if (departureTime >= targetDepTime) {
          startIndex = mid;
          right = mid - 1;
        } else {
          left = mid + 1;
        }
      }
    }

    for (int i = startIndex; i < tripCount && departures.size() < 20; ++i) {
      uint32_t tripOffset = scheduleOffset + 2 + i * 21;
      std::optional<Departure> dep = getDeparture(tripOffset, activeCalendar, currentUnixDay);
      if (dep && dep.value().departureUnixTime <= unixTimeUntil) departures.push_back(dep.value());
    }
    if (departures.size() == 20) break;
  }

  std::sort(departures.begin(), departures.end(),
            [](const Departure &a, const Departure &b) {
              return a.departureUnixTime < b.departureUnixTime;
            });

  std::list<Departure> departuresQueue;
  for (const auto &dep : departures) {
    departuresQueue.push_back(dep);
  }

  return departuresQueue;
}

std::vector<std::string> TimetableLoader::readRoutePattern(uint8_t count,
                                                           uint32_t offset) {
  if (count == 0)
    return {};

  if (!source.seek(offset)) {
    throw std::runtime_error("Failed to seek to route pattern");
  }

  std::vector<uint32_t> offsets(count);
  if (source.read(reinterpret_cast<uint8_t *>(offsets.data()), count * 4) !=
      count * 4) {
    throw std::runtime_error("Failed to read route pattern offsets");
  }

  std::vector<std::string> names;
  names.reserve(count);
  for (uint8_t i = 0; i < count; ++i) {
    names.push_back(readString(offsets[i]));
  }
  return names;
}

std::vector<uint16_t> TimetableLoader::readArrivalTimePattern(uint8_t count,
                                                              uint32_t offset) {
  if (count == 0)
    return {};

  if (!source.seek(offset)) {
    throw std::runtime_error("Failed to seek to arrival time pattern");
  }

  std::vector<uint16_t> times(count);
  if (source.read(reinterpret_cast<uint8_t *>(times.data()), count * 2) !=
      count * 2) {
    throw std::runtime_error("Failed to read arrival time pattern");
  }

  return times;
}

std::vector<ArriveStop>
TimetableLoader::getDetailedInfo(uint8_t count, uint64_t departureTime,
                                 uint32_t routePatternOffset,
                                 uint32_t arrivalTimePatternOffset) {
  std::vector<ArriveStop> details;
  if (count == 0)
    return details;

  std::vector<std::string> routePattern =
      readRoutePattern(count, routePatternOffset);
  std::vector<uint16_t> arrivalTimePattern =
      readArrivalTimePattern(count, arrivalTimePatternOffset);

  details.reserve(count);
  for (uint8_t i = 0; i < count; ++i) {
    details.push_back(
        {routePattern[i],
         static_cast<int>(departureTime + arrivalTimePattern[i] * 60)});
  }

  return details;
}
