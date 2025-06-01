#include <filesystem>
#include <unordered_map>
#include <memory>
#include <string>
#include "topic/topic.h"
#include "topic/topic_manager.h"
#include <spdlog/spdlog.h>

TopicManager::TopicManager(const std::string& root_dir, size_t default_partitions)
    : root_dir_(root_dir), default_partitions_(default_partitions) {
    std::filesystem::create_directories(root_dir_);
}

Topic& TopicManager::get_or_create_topic(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    spdlog::debug("Getting or creating topic: {}", name);
    auto it = topics_.find(name);
    if (it != topics_.end()) {
        return *it->second;
    }
    auto topic = std::make_unique<Topic>(name, root_dir_, default_partitions_);
    Topic* ptr = topic.get();
    topics_[name] = std::move(topic);
    spdlog::debug("Created topic: {}", name);
    return *ptr;
}

uint64_t TopicManager::produce(const std::string& topic, const std::string& key, const Record& record) {
    spdlog::debug("Producing record to topic: {}", topic);
    return get_or_create_topic(topic).append(key, record);
}

Record TopicManager::consume(const std::string& topic, const std::string& key, uint64_t offset) {
    spdlog::debug("Consuming record from topic: {}", topic);
    return get_or_create_topic(topic).read(key, offset);
}

Record TopicManager::consume(const std::string& topic, uint32_t partition_id, uint64_t offset) {
    spdlog::debug("Consuming record from topic: {} partition: {}", topic, partition_id);
    return get_or_create_topic(topic).read(partition_id, offset);
}
