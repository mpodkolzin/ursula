#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "broker/broker.h"
#include "record/record.h"
#include "offset_store/offset_store.h"
#include "client/broker_client.h"

class ConsumerGroup {
public:
    ConsumerGroup(const std::string& group_id, std::shared_ptr<BrokerConsumerClient> client);

    void subscribe(const std::string& topic);
    std::optional<Record> poll(const std::string& topic, uint32_t partition_id);
    void commit(const std::string& topic, uint32_t partition_id);
    uint64_t get_offset(const std::string& topic, uint32_t partition_id);

private:
    struct ConsumerState {
        std::unordered_map<std::string, std::unordered_map<uint32_t, uint64_t>> offsets;
        std::unordered_map<std::string, std::unordered_set<uint32_t>> assigned_partitions;
    };

    std::string offset_key(const std::string& topic, uint32_t partition_id) const;

    std::string group_id_;
    std::mutex mutex_;
    std::shared_ptr<BrokerConsumerClient> client_;
    std::unordered_map<std::string, ConsumerState> consumers_;
    std::unordered_set<std::string> subscriptions_;
    std::unordered_map<std::string, std::unordered_map<uint32_t, uint64_t>> in_memory_offsets_;


};
