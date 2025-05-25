#pragma once

#include "io/file_handle.h"
#include <vector>
#include <cstdint>
#include <mutex>

class BufferedWriter {
public:
    explicit BufferedWriter(FileHandle& file, size_t buffer_capacity = 4096);

    void write(const std::vector<uint8_t>& data);
    void write_u32(uint32_t value);
    void flush();

    size_t current_offset() const;

private:
    FileHandle& file_;
    std::vector<uint8_t> buffer_;
    size_t buffer_capacity_ = 4096;
    size_t written_offset_;
    mutable std::mutex mutex_;

    void write_bytes(const uint8_t* data, size_t size);
};
