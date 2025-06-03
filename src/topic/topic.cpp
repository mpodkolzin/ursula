#include <functional>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "partition/partition.h"
#include "record/record.h"
#include "topic/topic.h"
#include <spdlog/spdlog.h>
Topic::Topic(const std::string& name, const std::string& root_dir, size_t num_partitions)
    : name_(name), root_dir_(root_dir), num_partitions_(num_partitions) {

    std::string topic_path = root_dir_ + "/" + name_;
    std::filesystem::create_directories(topic_path);

    spdlog::info("Creating topic: {} with {} partitions", name_, num_partitions_);
    for (uint32_t pid = 0; pid < static_cast<uint32_t>(num_partitions_); ++pid) {
        std::string partition_path = topic_path + "/partition_" + std::to_string(pid);
        partitions_[pid] = std::make_unique<Partition>(partition_path);
        workers_[pid] = std::make_unique<PartitionWorker>(*partitions_[pid]);
    }
}

void Topic::start_workers() {
    for (auto& worker : workers_) {
        worker.second->start();
    }
}

void Topic::stop_workers() {
    for (auto& worker : workers_) {
        worker.second->stop();
    }
}

Topic::~Topic() {
    stop_workers();
}

uint64_t Topic::append(const std::string& key, const Record& record) {
    spdlog::debug("Appending record to topic: {} key: {}", name_, key);
    uint32_t pid = hash_to_partition(key);
    return get_partition(pid).append(record);
}

std::future<uint64_t> Topic::append_async(const std::string& key, const Record& record) {
    uint32_t pid = hash_to_partition(key);
    auto& worker = workers_[pid];
    if (!worker) {
        throw std::runtime_error("PartitionWorker not found for partition: " + std::to_string(pid));
    }
    return worker->enqueue(record);
}


Record Topic::read(const std::string& key, uint64_t offset) {
    spdlog::debug("Reading record from topic: {} key: {}", name_, key);
    uint32_t pid = hash_to_partition(key);
    return get_partition(pid).read(offset);
}

Record Topic::read(uint32_t partition_id, uint64_t offset) {
    spdlog::debug("Reading record from topic: {} partition: {} offset: {}", name_, partition_id, offset);
    return get_partition(partition_id).read(offset);
}

uint32_t Topic::hash_to_partition(const std::string& key) const {
    spdlog::debug("Hashing key: {} to partition", key);
    return std::hash<std::string>{}(key) % num_partitions_;
}

Partition& Topic::get_partition(uint32_t pid) {
    auto it = partitions_.find(pid);
    if (it == partitions_.end()) {
        throw std::runtime_error("Partition not found");
    }
    return *it->second;
}
