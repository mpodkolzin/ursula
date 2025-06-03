#include "consumer/consumer_group_manager.h"
#include "client/broker_client.h"
#include "offset_store/offset_store.h"



ConsumerGroupManager::ConsumerGroupManager(
    std::shared_ptr<BrokerConsumerClient> client,
    std::shared_ptr<ConsumerOffsetStore> store)
    : client_(std::move(client)), store_(std::move(store)) {}

std::shared_ptr<ConsumerGroup> ConsumerGroupManager::get_or_create(const std::string& group_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = groups_.find(group_id);
    if (it != groups_.end()) return it->second;

    auto group = std::make_shared<ConsumerGroup>(group_id, client_);
    groups_[group_id] = group;
    return group;
}
