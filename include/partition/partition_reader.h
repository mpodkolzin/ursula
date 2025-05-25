#pragma once

#include <map>
#include <memory>
#include <string>
#include "partition/log_segment.h"
#include "record/record.h"

class PartitionReader {
public:
    explicit PartitionReader(const std::map<uint64_t, std::unique_ptr<LogSegment>>& segments);

    Record read(uint64_t offset);

private:
    const std::map<uint64_t, std::unique_ptr<LogSegment>>& segments_;
};