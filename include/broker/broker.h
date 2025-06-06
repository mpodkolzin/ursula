#pragma once

#include <string>
#include <vector>
#include <memory>
#include "partition/partition_manager.h"
#include "metrics/metrics_collector.h"
#include "topic/topic_manager.h"
#include "offset_store/offset_store.h"
#include "producer/partition_worker.h"


class Broker {
public:
    Broker(const Broker&) = delete;
    Broker& operator=(const Broker&) = delete;
    explicit Broker(const std::string& data_dir, size_t default_partitions, std::shared_ptr<ConsumerOffsetStore> offset_store);
    // For testing
    explicit Broker(const std::string& data_dir, std::unique_ptr<MetricsCollector> metrics_override, std::shared_ptr<ConsumerOffsetStore> offset_store);

    uint64_t produce(const std::string& topic, const std::string& key, const Record& record);
    Record consume(const std::string& topic, const std::string& key, uint64_t offset);
    Record consume(const std::string& topic, uint32_t partition_id, uint64_t offset);
    void commit_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id, uint64_t offset);
    uint64_t get_committed_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id);

    const MetricsCollector& metrics() const;

    //alternative producer implementation
    void produce_async(const std::string& topic, const std::string& key, const Record& record);

private:
    std::unordered_map<std::string, std::unordered_map<uint32_t, std::unique_ptr<PartitionWorker>>> workers_;
    std::mutex workers_mutex_;

    std::unique_ptr<TopicManager> topic_manager_;
    std::unique_ptr<MetricsCollector> metrics_;
    mutable std::mutex offset_mutex_;
    std::unordered_map<std::string,  // group_id
    std::unordered_map<std::string,  // topic
        std::unordered_map<uint32_t, uint64_t>  // partition_id -> offset
    >
    > offset_table_;

    std::shared_ptr<ConsumerOffsetStore> offset_store_;
};
