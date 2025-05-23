#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include "io/file_handle.h"

class PartitionReader {
public:
    PartitionReader(const std::string& segment_dir);
    std::vector<uint8_t> read(uint64_t offset);

private:
    uint64_t base_offset_;
    std::unique_ptr<FileHandle> log_file_;
    std::unique_ptr<FileHandle> index_file_;

    std::string segment_dir_;

    uint32_t find_log_position(uint64_t offset);
};
