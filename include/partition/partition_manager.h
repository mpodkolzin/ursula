#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include "partition/partition_writer.h"
//#include "PartitionReader.h"

using PartitionId = uint32_t;

class PartitionManager {
public:
    explicit PartitionManager(const std::string& data_dir);

    uint64_t append(PartitionId pid, const std::vector<uint8_t>& message);
    //std::vector<uint8_t> read(PartitionId pid, uint64_t offset);

    void flush_all();

private:
    std::string data_dir_;
    std::mutex mutex_;
    std::unordered_map<PartitionId, std::unique_ptr<PartitionWriter>> writers_;
    //std::unordered_map<PartitionId, std::unique_ptr<PartitionReader>> readers_;

    void create_partition_if_missing(PartitionId pid);
};
