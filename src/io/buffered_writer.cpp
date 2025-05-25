
#include "io/buffered_writer.h"
#include "io/file_handle.h"
#include <vector>
#include <cstdint>
#include <mutex>
#include <cstring>

BufferedWriter::BufferedWriter(FileHandle& file, size_t buffer_capacity)
    : file_(file), buffer_(), buffer_capacity_(buffer_capacity), written_offset_(file_.seek(0, SEEK_CUR)) {
    buffer_.reserve(buffer_capacity_);
}

void BufferedWriter::write(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() + data.size() > buffer_capacity_) {
        flush();
    }
    buffer_.insert(buffer_.end(), data.begin(), data.end());
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!buffer_.empty()) {
        ssize_t written = file_.write(buffer_.data(), buffer_.size());
        if (written != static_cast<ssize_t>(buffer_.size())) {
            throw std::runtime_error("Failed to write full buffer to file");
        }
        written_offset_ += written;
        buffer_.clear();
    }
}

size_t BufferedWriter::current_offset() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return written_offset_ + buffer_.size();
}


void BufferedWriter::write_bytes(const uint8_t* data, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() + size > buffer_capacity_) {
        flush();
    }
    buffer_.insert(buffer_.end(), data, data + size);
}
