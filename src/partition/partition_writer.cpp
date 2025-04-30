#include "partition/partition_writer.h"
#include <filesystem>
#include <fcntl.h>      
#include <unistd.h>     
#include <cstring>      
#include <stdexcept>

PartitionWriter::PartitionWriter(const std::string& directory, uint64_t base_offset)
    : directory_(directory),
      next_offset_(base_offset),
      base_offset_(base_offset),
      segment_size_limit_(128 * 1024 * 1024), // 128 MB
      buffer_flush_threshold_(64 * 1024),     // 64 KB buffer flush threshold
      running_(true),
      flush_requested_(false),
      flush_interval_ms_(1000),
      bytes_written_in_segment_(0) {
    open_segment_files();

    flush_thread_ = std::thread(&PartitionWriter::flush_loop, this);
}

PartitionWriter::~PartitionWriter() {
    flush_cv_.notify_all();
    if (flush_thread_.joinable()) {
        flush_thread_.join();
    }
    flush();
    if (log_file_) {
        log_file_->close();
    }
    if (index_file_) {
        index_file_->close();
    }
}


void PartitionWriter::open_segment_files() {
    std::filesystem::create_directories(directory_);

    std::string base_name = directory_ + "/" + std::to_string(base_offset_);

    log_file_ = std::make_unique<FileHandle>(base_name + ".log", O_WRONLY | O_CREAT | O_APPEND);
    index_file_ = std::make_unique<FileHandle>(base_name + ".index", O_WRONLY | O_CREAT | O_APPEND);

    log_buffer_.clear();
    index_buffer_.clear();
}

uint64_t PartitionWriter::append(const std::vector<uint8_t>& message) {
    uint64_t assigned_offset = next_offset_;
    ++next_offset_;

    write_message(assigned_offset, message);

    if (log_buffer_.size() >= buffer_flush_threshold_) {
        std::lock_guard<std::mutex> lock(flush_mutex_);
        flush_requested_ = true;
        flush_cv_.notify_one();
    }

    if (bytes_written_in_segment_ >= segment_size_limit_) {
        roll_segment();
    }

    return assigned_offset;
}


void PartitionWriter::write_message(uint64_t offset, const std::vector<uint8_t>& message) {
    uint32_t message_size = static_cast<uint32_t>(message.size());
    uint32_t pos_in_log = static_cast<uint32_t>(log_file_->seek(0, SEEK_CUR)) + static_cast<uint32_t>(log_buffer_.size());

    // Append [message_size][message] to log_buffer_
    const uint8_t* size_ptr = reinterpret_cast<const uint8_t*>(&message_size);
    log_buffer_.insert(log_buffer_.end(), size_ptr, size_ptr + sizeof(message_size));
    log_buffer_.insert(log_buffer_.end(), message.begin(), message.end());

    // Append [relative_offset][position] to index_buffer_
    uint32_t relative_offset = static_cast<uint32_t>(offset - base_offset_);
    const uint8_t* offset_ptr = reinterpret_cast<const uint8_t*>(&relative_offset);
    const uint8_t* pos_ptr = reinterpret_cast<const uint8_t*>(&pos_in_log);

    index_buffer_.insert(index_buffer_.end(), offset_ptr, offset_ptr + sizeof(relative_offset));
    index_buffer_.insert(index_buffer_.end(), pos_ptr, pos_ptr + sizeof(pos_in_log));

    bytes_written_in_segment_ += sizeof(message_size) + message.size();
}

void PartitionWriter::flush() {
    flush_buffers();
    log_file_->flush();
    index_file_->flush();
}

void PartitionWriter::flush_buffers() {
    if (!log_buffer_.empty()) {
        if (log_file_->write(log_buffer_.data(), log_buffer_.size()) != static_cast<ssize_t>(log_buffer_.size())) {
            throw std::runtime_error("Failed to flush log buffer");
        }
        log_buffer_.clear();
    }

    if (!index_buffer_.empty()) {
        if (index_file_->write(index_buffer_.data(), index_buffer_.size()) != static_cast<ssize_t>(index_buffer_.size())) {
            throw std::runtime_error("Failed to flush index buffer");
        }
        index_buffer_.clear();
    }
}


void PartitionWriter::flush_loop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(flush_mutex_);
        flush_cv_.wait(lock,
         [this] { return flush_requested_ || !running_; });

        if (!running_) {
            break;
        }

        flush_buffers();
        flush_requested_ = false;
    }
}

void PartitionWriter::roll_segment() {
    flush();
    log_file_->close();
    index_file_->close();

    base_offset_ = next_offset_;
    bytes_written_in_segment_ = 0;

    open_segment_files();
}
