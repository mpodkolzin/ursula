#include <sstream>
#include <iomanip>
#include <filesystem>
#include "partition/partition_writer.h"
#include "partition/log_segment.h"
#include "record/record.h"
#include <spdlog/spdlog.h>

PartitionWriter::PartitionWriter(std::map<uint64_t, std::unique_ptr<LogSegment>>& segments,
    const std::string& partition_path,
    size_t max_segment_size_bytes)
    : segments_(segments), partition_path_(partition_path), max_segment_size_bytes_(max_segment_size_bytes) {
    if (!segments_.empty()) {
        next_offset_ = segments_.rbegin()->first;
    }
}

uint64_t PartitionWriter::append(const Record& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (should_roll_segment()) {
        spdlog::debug("PartitionWriter::append: Rolling segment");
        roll_segment();
    }

    LogSegment& segment = active_segment();
    uint64_t offset = next_offset_++;
    segment.append(offset, record);
    return offset;
}

LogSegment& PartitionWriter::active_segment() {
    if (segments_.empty()) {
        roll_segment();
    }
    return *segments_.rbegin()->second;
}

void PartitionWriter::roll_segment() {
    uint64_t base_offset = next_offset_;
    std::ostringstream oss;
    oss << std::setw(20) << std::setfill('0') << base_offset;
    std::string segment_name = oss.str();
    std::string path = partition_path_ + "/" + segment_name;

    segments_[base_offset] = std::make_unique<LogSegment>(path, base_offset);
}

bool PartitionWriter::should_roll_segment() const {
    if (segments_.empty()) return true;
    const auto& segment = segments_.rbegin()->second;
    return segment->size() >= max_segment_size_bytes_;
}
