#pragma once

#include <string>
#include <vector>
#include <memory>
#include "partition/partition_manager.h"
#include "metrics/metrics_collector.h"
#include "topic/topic_manager.h"

class Broker {
public:
    explicit Broker(const std::string& data_dir, size_t default_partitions = 3);
    // For testing
    explicit Broker(const std::string& data_dir, std::unique_ptr<MetricsCollector> metrics_override);

    uint64_t produce(const std::string& topic, const std::string& key, const Record& record);
    Record consume(const std::string& topic, const std::string& key, uint64_t offset);

    const MetricsCollector& metrics() const;

private:
    std::unique_ptr<TopicManager> topic_manager_;
    std::unique_ptr<MetricsCollector> metrics_collector_;
};
