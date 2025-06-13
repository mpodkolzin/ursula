#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "record/record.h"
#include "io/file_handle.h"
#include "io/buffered_writer.h"
#include "io/record_reader.h"

class LogSegment {
public:
    LogSegment(const std::string& path, uint64_t base_offset);
    ~LogSegment();

    uint64_t base_offset() const;
    uint64_t append(uint64_t offset, const Record& record);
    Record read(uint64_t offset) const;
    bool contains(uint64_t offset) const;
    size_t size() const;

    void flush();
    void close();

private:
    uint64_t base_offset_;
    std::string path_;
    std::unique_ptr<FileHandle> log_file_raw_;
    std::unique_ptr<FileHandle> index_file_raw_;
    std::unique_ptr<BufferedWriter> log_writer_;
    std::unique_ptr<BufferedWriter> index_writer_;

    std::unique_ptr<RecordReader> record_reader_;


    mutable std::mutex flush_mutex_;
    std::condition_variable flush_cv_;
    std::thread flush_thread_;
    bool running_ = true;
    std::atomic<bool> flush_requested_ = false;

    void flush_loop();
    static std::string log_file_name(const std::string& path);
    static std::string index_file_name(const std::string& path);
};