#include <fstream>
#include <string>
#include "FileDataSource.h"

FileDataSource::FileDataSource(std::string filename) {
    file = new std::ifstream(filename, std::ios::binary);
}

FileDataSource::~FileDataSource() {
    if (file) {
        delete file;
    }
}

size_t FileDataSource::read(uint8_t *buffer, size_t size) {
    file->read(reinterpret_cast<std::istream::char_type *>(buffer), size);
    return file->gcount();
}

bool FileDataSource::seek(uint32_t position) {
    file->clear();
    file->seekg(position, std::ifstream::beg);
    return file->good();
}

bool FileDataSource::isOpen() {
    return file->is_open();
}
