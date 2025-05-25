#include "partition/log_segment.h"
#include "io/file_handle.h"
#include "io/buffered_writer.h"
#include "io/record_reader.h"
#include <filesystem>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <fcntl.h>


LogSegment::LogSegment(const std::string& path, uint64_t base_offset)
    : base_offset_(base_offset) {
    std::filesystem::create_directories(path);
    log_file_raw_ = std::make_unique<FileHandle>(log_file_name(path), O_CREAT | O_RDWR, 0644);
    index_file_raw_ = std::make_unique<FileHandle>(index_file_name(path), O_CREAT | O_RDWR, 0644);
    log_writer_ = std::make_unique<BufferedWriter>(*log_file_raw_, 4096);
    index_writer_ = std::make_unique<BufferedWriter>(*index_file_raw_, 4096);
    flush_thread_ = std::thread(&LogSegment::flush_loop, this);
    record_reader_ = std::make_unique<RecordReader>(*log_file_raw_);
}

LogSegment::~LogSegment() {
    {
        std::lock_guard<std::mutex> lock(flush_mutex_);
        running_ = false;
        flush_requested_ = true;
    }
    flush_cv_.notify_all();
    if (flush_thread_.joinable()) {
        flush_thread_.join();
    }
    flush();
}

uint64_t LogSegment::base_offset() const {
    return base_offset_;
}

bool LogSegment::contains(uint64_t offset) const {
    return offset >= base_offset_;
}
uint64_t LogSegment::append(uint64_t offset, const Record& record) {
    spdlog::info("LogSegment::append: offset='{}'", offset);
    uint32_t relative_offset = static_cast<uint32_t>(offset - base_offset_);
    uint32_t pos_in_log = static_cast<uint32_t>(log_writer_->current_offset());

    std::vector<uint8_t> data = record.serialize();
    log_writer_->write(data);

    index_writer_->write_u32(relative_offset);
    index_writer_->write_u32(pos_in_log);

    // Notify flush thread
    {
        std::lock_guard<std::mutex> lock(flush_mutex_);
        flush_requested_ = true;
    }
    flush_cv_.notify_all();

    return offset;
}

Record LogSegment::read(uint64_t offset) const {
    spdlog::info("LogSegment::read: offset='{}'", offset);
    uint32_t relative = static_cast<uint32_t>(offset - base_offset_);
    size_t entry_size = sizeof(uint32_t) * 2;
    size_t index_size = index_file_raw_->file_size();
    size_t entry_count = index_size / entry_size;
    spdlog::info("LogSegment::read: entry_count='{}'", entry_count);

    for (size_t i = 0; i < entry_count; ++i) {
        index_file_raw_->seek(i * entry_size, SEEK_SET);
        uint32_t ro, pos;
        index_file_raw_->read(&ro, sizeof(ro));
        index_file_raw_->read(&pos, sizeof(pos));
        spdlog::info("LogSegment::read: ro='{}', pos='{}'", ro, pos);
        if (ro == relative) {
            spdlog::info("LogSegment::read: found offset='{}'", offset);
            return record_reader_->read_at(pos);  // 🔁 now uses the helper
        }
    }

    throw std::runtime_error("Offset not found in index");
}


size_t LogSegment::size() const {
    return log_file_raw_->file_size();
}

void LogSegment::flush() {
    log_writer_->flush();
    index_writer_->flush();
}

void LogSegment::close() {
    flush();
    //this is ugly, need to move this to writer class
    log_file_raw_->close();
    index_file_raw_->close();
}

void LogSegment::flush_loop() {
    using namespace std::chrono_literals;
    while (true) {
        std::unique_lock<std::mutex> lock(flush_mutex_);
        flush_cv_.wait_for(lock, 100ms, [this] {
            return flush_requested_ || !running_;
        });

        if (!running_) break;

        if (flush_requested_) {
            flush();
            flush_requested_ = false;
        }
    }
}

std::string LogSegment::log_file_name(const std::string& path) {
    return path + ".log";
}

std::string LogSegment::index_file_name(const std::string& path) {
    return path + ".index";
}
