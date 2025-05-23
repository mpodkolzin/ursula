#include "partition/partition_writer.h"
#include <filesystem>
#include <fcntl.h>      
#include <unistd.h>     
#include <cstring>      
#include <stdexcept>
#include "util/byteio.h"

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
    running_ = false;
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

uint64_t PartitionWriter::append(const Record& record) {
    uint64_t assigned_offset = next_offset_++;
    write_record(assigned_offset, record);

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


void PartitionWriter::write_record(uint64_t offset, const Record& record) {
    std::vector<uint8_t> bytes = record.serialize();
    uint32_t pos_in_log = static_cast<uint32_t>(log_file_->seek(0, SEEK_CUR)) + static_cast<uint32_t>(log_buffer_.size());

    log_buffer_.insert(log_buffer_.end(), bytes.begin(), bytes.end());

    uint32_t relative_offset = static_cast<uint32_t>(offset - base_offset_);

    write_u32(index_buffer_, relative_offset);
    write_u32(index_buffer_, pos_in_log);

    bytes_written_in_segment_ += bytes.size();
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
        flush_cv_.wait_for(lock, 
        std::chrono::milliseconds(flush_interval_ms_),
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
