#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include "consumer/consumer_group.h"
#include "client/broker_client.h"
#include "offset_store/offset_store.h"



class ConsumerGroupManager {
public:
    ConsumerGroupManager(std::shared_ptr<BrokerConsumerClient> client,
                         std::shared_ptr<ConsumerOffsetStore> store);

    std::shared_ptr<ConsumerGroup> get_or_create(const std::string& group_id);

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<ConsumerGroup>> groups_;
    std::shared_ptr<BrokerConsumerClient> client_;
    std::shared_ptr<ConsumerOffsetStore> store_;
};
