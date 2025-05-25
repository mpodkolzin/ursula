#include <map>
#include <memory>
#include <string>
#include "partition/log_segment.h"
#include "record/record.h"
#include "partition/partition_reader.h"
#include <stdexcept>


PartitionReader::PartitionReader(const std::map<uint64_t, std::unique_ptr<LogSegment>>& segments)
    : segments_(segments) {}

Record PartitionReader::read(uint64_t offset) {
    for (auto it = segments_.rbegin(); it != segments_.rend(); ++it) {
        if (it->second->contains(offset)) {
            return it->second->read(offset);
        }
    }
    throw std::runtime_error("Offset not found in any segment");
}
