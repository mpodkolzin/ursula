#pragma once

#include <map>
#include <memory>
#include <string>
#include "partition/log_segment.h"
#include "record/record.h"

class PartitionWriter {
public:
    PartitionWriter(std::map<uint64_t, std::unique_ptr<LogSegment>>& segments, const std::string& partition_path, size_t max_segment_size_bytes = 10 * 1024 * 1024);

    uint64_t append(const Record& record);

private:
    std::mutex mutex_;
    std::map<uint64_t, std::unique_ptr<LogSegment>>& segments_;
    std::string partition_path_;
    uint64_t next_offset_ = 0;
    size_t max_segment_size_bytes_;

    LogSegment& active_segment();
    void roll_segment();
    bool should_roll_segment() const;
};