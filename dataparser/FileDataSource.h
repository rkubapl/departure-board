#pragma once
#include <fstream>
#include <string>
#include "IDataSource.h"

//NOTE: temporary file, use LVGL file system later

class FileDataSource : public IDataSource {
  std::ifstream *file;

public:
  FileDataSource(std::string filename) {
    file = new std::ifstream(filename, std::ios::binary);
  }
  
  ~FileDataSource() { 
      if (file) {
          delete file; 
      }
  }
  
  size_t read(uint8_t *buffer, size_t size) override {
    file->read(reinterpret_cast<std::istream::char_type *>(buffer), size);
    return file->gcount();
  }

  bool seek(uint32_t position) override {
    file->clear();
    file->seekg(position, std::ifstream::beg);
    return file->good();
  }
  
  bool is_open() const {
      return file->is_open();
  }
};
