#include "partition/log_segment.h"
#include "io/file_handle.h"
#include "io/buffered_writer.h"
#include "io/record_reader.h"
#include <filesystem>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <fcntl.h>


LogSegment::LogSegment(const std::string& path, uint64_t base_offset)
    : base_offset_(base_offset),
    path_(path) {
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
    // Removed per-record debug logging (do not re-enable during benchmarking)

    std::string threadIdStr = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    uint32_t relative_offset = static_cast<uint32_t>(offset - base_offset_);
    uint32_t pos_in_log = static_cast<uint32_t>(log_writer_->current_offset());

    // Reuse thread-local buffer to avoid per-record heap allocation
    thread_local std::vector<uint8_t> buffer;
    buffer.clear();
    record.serialize_into(buffer);

    log_writer_->write(buffer);

    index_writer_->write_u32(relative_offset);
    index_writer_->write_u32(pos_in_log);

    // Notify flush thread only if not already requested
    bool expected = false;
    if (flush_requested_.compare_exchange_strong(expected, true)) {
        flush_cv_.notify_one();  // notify_one is cheaper than notify_all
    }

    return offset;
}


Record LogSegment::read(uint64_t offset) const {
    // Removed per-record debug logging (do not re-enable during benchmarking)
    uint32_t relative = static_cast<uint32_t>(offset - base_offset_);
    size_t entry_size = sizeof(uint32_t) * 2;
    size_t index_size = index_file_raw_->file_size();
    size_t entry_count = index_size / entry_size;

    for (size_t i = 0; i < entry_count; ++i) {
        index_file_raw_->seek(i * entry_size, SEEK_SET);
        uint32_t ro, pos;
        index_file_raw_->read(&ro, sizeof(ro));
        index_file_raw_->read(&pos, sizeof(pos));
        if (ro == relative) {
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
    //pthread_setname_np(pthread_self(), "flush_loop");
    while (true) {
        std::unique_lock<std::mutex> lock(flush_mutex_);
        //spdlog::trace("flush_loop: waiting on flush_cv_...");

        bool wake_due_to_condition = flush_cv_.wait_for(lock, 1000ms, [this] {
            return flush_requested_ || !running_;
        });

        if (!running_) {
            //spdlog::trace("flush_loop: exiting, running_ = false");
            break;
        }

        if (!wake_due_to_condition) {
            //spdlog::trace("flush_loop: woke up due to timeout");
        } else if (flush_requested_) {
            //spdlog::trace("flush_loop: flush_requested_ = true, flushing now");
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
