#pragma once

#include <map>
#include <memory>
#include <string>
#include "record/record.h"
#include "partition/log_segment.h"
#include "partition/partition_writer.h"
#include "partition/partition_reader.h"

class Partition {
public:
    Partition(const std::string& path);

    uint64_t append(const Record& record);
    Record read(uint64_t offset);

private:

    std::mutex mutex_;
    std::map<uint64_t, std::unique_ptr<LogSegment>> segments_;
    std::unique_ptr<PartitionWriter> writer_;
    std::unique_ptr<PartitionReader> reader_;
    std::string path_;

    void load_segments();
    LogSegment* find_segment(uint64_t offset);
};