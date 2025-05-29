#include "broker/broker.h"
#include <spdlog/spdlog.h>
#include "client/broker_client.h"
#include "offset_store/offset_store.h"


Broker::Broker(const std::string& data_dir, size_t default_partitions, std::unique_ptr<OffsetStore> offset_store)
    : metrics_(std::make_unique<MetricsCollector>()),
      topic_manager_(std::make_unique<TopicManager>(data_dir, default_partitions)),
      offset_store_(std::move(offset_store)) {}

uint64_t Broker::produce(const std::string& topic, const std::string& key, const Record& record) {
    metrics_->increment_produced();
    return topic_manager_->produce(topic, key, record);
}

Record Broker::consume(const std::string& topic, const std::string& key, uint64_t offset) {
    metrics_->increment_consumed();
    return topic_manager_->consume(topic, key, offset);
}

Record Broker::consume(const std::string& topic, uint32_t partition_id, uint64_t offset) {
    metrics_->increment_consumed();
    return topic_manager_->consume(topic, partition_id, offset);
}

void Broker::commit_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id, uint64_t offset) {
    std::lock_guard<std::mutex> lock(offset_mutex_);
    offset_table_[group_id][topic][partition_id] = offset;
}

uint64_t Broker::get_committed_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id) const {
    std::lock_guard<std::mutex> lock(offset_mutex_);
    auto group_it = offset_table_.find(group_id);
    if (group_it != offset_table_.end()) {
        auto topic_it = group_it->second.find(topic);
        if (topic_it != group_it->second.end()) {
            auto part_it = topic_it->second.find(partition_id);
            if (part_it != topic_it->second.end()) {
                return part_it->second;
            }
        }
    }
    return 0;
}

const MetricsCollector& Broker::metrics() const {
    return *metrics_;
}