#pragma once

#include <string>
#include <vector>
#include <memory>
#include "io/file_handle.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include "record/record.h"

class PartitionWriter {
public:
    PartitionWriter(const std::string& directory, uint64_t base_offset);
    ~PartitionWriter();

    uint64_t append(const Record& record);
    void flush();
    void roll_segment();

private:
    std::string directory_;
    uint64_t next_offset_;
    uint64_t base_offset_;
    size_t segment_size_limit_;
    size_t buffer_flush_threshold_;
    uint64_t flush_interval_ms_;

    std::unique_ptr<FileHandle> log_file_;
    std::unique_ptr<FileHandle> index_file_;

    std::vector<uint8_t> log_buffer_;
    std::vector<uint8_t> index_buffer_;
    size_t bytes_written_in_segment_;

    std::thread flush_thread_;
    std::mutex flush_mutex_;
    std::condition_variable flush_cv_;
    std::atomic<bool> flush_requested_;
    std::atomic<bool> running_;

    bool sync_on_flush_;


    void open_segment_files();
    void write_record(uint64_t offset, const Record& message);
    void write_index_entry(uint32_t relative_offset, uint32_t position);

    void flush_buffers();
    void flush_loop();
};
