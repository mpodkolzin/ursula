#pragma once

#include "io/file_handle.h"
#include "record/record.h"
#include <cstdint>
#include <vector>
#include <memory>

class RecordReader {
public:
    explicit RecordReader(FileHandle& file);

    Record read_at(uint32_t offset);

//TODO move away from raw file handle
private:
    FileHandle& file_;
};