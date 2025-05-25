#include <functional>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "partition/partition.h"
#include "record/record.h"
#include "topic/topic.h"

Topic::Topic(const std::string& name, const std::string& root_dir, size_t num_partitions)
    : name_(name), root_dir_(root_dir), num_partitions_(num_partitions) {

    std::string topic_path = root_dir_ + "/" + name_;
    std::filesystem::create_directories(topic_path);

    for (uint32_t pid = 0; pid < static_cast<uint32_t>(num_partitions_); ++pid) {
        std::string partition_path = topic_path + "/partition_" + std::to_string(pid);
        partitions_[pid] = std::make_unique<Partition>(partition_path);
    }
}

uint64_t Topic::append(const std::string& key, const Record& record) {
    uint32_t pid = hash_to_partition(key);
    return get_partition(pid).append(record);
}

Record Topic::read(const std::string& key, uint64_t offset) {
    uint32_t pid = hash_to_partition(key);
    return get_partition(pid).read(offset);
}

uint32_t Topic::hash_to_partition(const std::string& key) const {
    return std::hash<std::string>{}(key) % num_partitions_;
}

Partition& Topic::get_partition(uint32_t pid) {
    auto it = partitions_.find(pid);
    if (it == partitions_.end()) {
        throw std::runtime_error("Partition not found");
    }
    return *it->second;
}
