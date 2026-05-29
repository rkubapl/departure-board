#pragma once
#include <fstream>
#include <string>
#include "IDataSource.h"

class FileDataSource : public IDataSource {
  std::ifstream *file;

  public:
    FileDataSource(std::string filename);
    ~FileDataSource();
  
    size_t read(uint8_t *buffer, size_t size) override;
    bool seek(uint32_t position) override;
    bool isOpen() override;
};
