
#include "io/buffered_writer.h"
#include "io/file_handle.h"
#include <vector>
#include <cstdint>
#include <mutex>
#include <cstring>
#include <spdlog/spdlog.h>

BufferedWriter::BufferedWriter(FileHandle& file, size_t buffer_capacity)
: file_(file), buffer_capacity_(buffer_capacity), written_offset_(file_.seek(0, SEEK_CUR)) {
buffer_.reserve(buffer_capacity_);
}

void BufferedWriter::write(const std::vector<uint8_t>& data) {
    bool needs_flush = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::trace("BufferedWriter::write: Acquired lock");

        if (buffer_.size() + data.size() > buffer_capacity_) {
            needs_flush = true;
        } else {
            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }
    }

    if (needs_flush) {
        flush();
        // Retry after flush
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.insert(buffer_.end(), data.begin(), data.end());
    }
}

void BufferedWriter::write_u32(uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = (value >> 0) & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    bytes[3] = (value >> 24) & 0xFF;
    write_bytes(bytes, sizeof(bytes));
}

void BufferedWriter::flush() {
    std::vector<uint8_t> temp;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.empty()) return;
        spdlog::trace("BufferedWriter::flush: Moving buffer for flush");
        temp.swap(buffer_);  // Efficient swap
    }

    spdlog::trace("BufferedWriter::flush: Writing to file");
    ssize_t written = file_.write(temp.data(), temp.size());
    if (written != static_cast<ssize_t>(temp.size())) {
        throw std::runtime_error("Failed to write full buffer to file");
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        written_offset_ += written;
    }
}

size_t BufferedWriter::current_offset() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return written_offset_ + buffer_.size();
}

void BufferedWriter::write_bytes(const uint8_t* data, size_t size) {
    std::vector<uint8_t> vec(data, data + size);
    write(vec);  // Reuse logic
}