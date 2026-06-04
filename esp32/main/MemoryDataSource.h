#pragma once
#include "../../dataparser/IDataSource.h"

class MemoryDataSource : public IDataSource {
    const uint8_t* data;
    size_t size;
    size_t position;

public:
    MemoryDataSource(const uint8_t* data, size_t size);
    ~MemoryDataSource() override = default;

    size_t read(uint8_t* buffer, size_t readSize) override;
    bool seek(uint32_t pos) override;
    bool isOpen() override;
};
