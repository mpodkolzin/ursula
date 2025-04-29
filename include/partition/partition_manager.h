#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>

using PartitionId = uint64_t;

class PartitionManager {
public:
    PartitionManager(const std::string& data_dir);

    uint64_t append(PartitionId pid, const std::vector<uint8_t>& message);
    void flush_all();
    void roll_partitions_if_needed();
    void create_partition(PartitionId pid);
    bool has_partition(PartitionId pid) const;

private:
    std::string data_dir_;
    std::unordered_map<PartitionId, std::unique_ptr<PartitionWriter>> partitions_;
    mutable std::mutex partitions_mutex_;
};