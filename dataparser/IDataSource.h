#pragma once
#include <cstdint>
#include <cstddef>

class IDataSource {
public:
    virtual ~IDataSource() = default;

    virtual size_t read(uint8_t* buffer, size_t size) = 0;
    virtual bool seek(uint32_t position) = 0;
    virtual bool isOpen() = 0;
};