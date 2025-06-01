#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "topic.h"

class TopicManager {
public:
    TopicManager(const TopicManager&) = delete;
    TopicManager& operator=(const TopicManager&) = delete;
    explicit TopicManager(const std::string& root_dir, size_t default_partitions);

    uint64_t produce(const std::string& topic, const std::string& key, const Record& record);
    Record consume(const std::string& topic, const std::string& key, uint64_t offset);
    Record consume(const std::string& topic, uint32_t partition_id, uint64_t offset);
private:

    Topic& get_or_create_topic(const std::string& name);
    std::string root_dir_;
    size_t default_partitions_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<Topic>> topics_;
};