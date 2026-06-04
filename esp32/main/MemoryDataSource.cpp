#include "MemoryDataSource.h"
#include <cstring>
#include <algorithm>

MemoryDataSource::MemoryDataSource(const uint8_t* data, size_t size)
    : data(data), size(size), position(0) {
}

size_t MemoryDataSource::read(uint8_t* buffer, size_t readSize) {
    if (!data || position >= size) {
        return 0;
    }
    size_t bytesToRead = std::min(readSize, size - position);
    std::memcpy(buffer, data + position, bytesToRead);
    position += bytesToRead;
    return bytesToRead;
}

bool MemoryDataSource::seek(uint32_t pos) {
    if (!data || pos > size) {
        return false;
    }
    position = pos;
    return true;
}

bool MemoryDataSource::isOpen() {
    return data != nullptr;
}
