#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "broker/broker.h"
#include "record/record.h"
#include "consumer/consumer_group.h"
#include "client/broker_client.h"
#include "offset_store/offset_store.h"

ConsumerGroup::ConsumerGroup(const std::string& group_id,
                             std::shared_ptr<BrokerConsumerClient> client)
    : group_id_(group_id),
      client_(std::move(client)) {}

void ConsumerGroup::subscribe(const std::string& topic) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscriptions_.insert(topic);
}

std::optional<Record> ConsumerGroup::poll(const std::string& topic, uint32_t partition_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (subscriptions_.find(topic) == subscriptions_.end()) {
        throw std::runtime_error("Topic not subscribed: " + topic);
    }

    uint64_t offset = get_offset(topic, partition_id);
    Record record = client_->consume(topic, partition_id, offset);

    // Update in-memory offset only
    in_memory_offsets_[topic][partition_id] = offset + 1;
    return record;
}

void ConsumerGroup::commit(const std::string& topic, uint32_t partition_id) {
    auto topic_it = in_memory_offsets_.find(topic);
    if (topic_it != in_memory_offsets_.end()) {
        auto part_it = topic_it->second.find(partition_id);
        if (part_it != topic_it->second.end()) {
            client_->commit_offset(group_id_, topic, partition_id, part_it->second);
    }
}
}

uint64_t ConsumerGroup::get_offset(const std::string& topic, uint32_t partition_id) {
    auto topic_it = in_memory_offsets_.find(topic);
    if (topic_it != in_memory_offsets_.end()) {
        auto part_it = topic_it->second.find(partition_id);
        if (part_it != topic_it->second.end()) {
            return part_it->second;
        }
    }
    return client_->get_committed_offset(group_id_, topic, partition_id);
}

std::string ConsumerGroup::offset_key(const std::string& topic, uint32_t partition_id) const {
    return topic + "." + std::to_string(partition_id);
}
