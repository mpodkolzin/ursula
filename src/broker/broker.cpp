#include "broker/broker.h"
#include <spdlog/spdlog.h>

Broker::Broker(const std::string& data_dir, size_t default_partitions) {
    topic_manager_ = std::make_unique<TopicManager>(data_dir, default_partitions);
    metrics_collector_ = std::make_unique<MetricsCollector>();
}

Broker::Broker(const std::string& data_dir, std::unique_ptr<MetricsCollector> metrics_override)
    : metrics_collector_(std::move(metrics_override)) {
    topic_manager_ = std::make_unique<TopicManager>(data_dir, 3);
}


uint64_t Broker::produce(const std::string& topic, const std::string& key, const Record& record) {
    spdlog::info("Broker::produce: topic='{}', key='{}'", topic, key);
    if (metrics_collector_ == nullptr) {
        spdlog::info("Broker::produce: metrics_collector_ is nullptr");
    }
    metrics_collector_->increment_produced();
    if (topic_manager_ == nullptr) {
        spdlog::info("Broker::produce: topic_manager_ is nullptr");
    }
    return topic_manager_->produce(topic, key, record);
}

Record Broker::consume(const std::string& topic, const std::string& key, uint64_t offset) {
    spdlog::info("Consuming from topic='{}', key='{}', offset={}", topic, key, offset);
    metrics_collector_->increment_consumed();
    return topic_manager_->consume(topic, key, offset);
}

const MetricsCollector& Broker::metrics() const {
    return *metrics_collector_;
}
