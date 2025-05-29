#include "client/broker_client.h"
#include "broker/broker.h"
#include "record/record.h"
#include <memory>
#include "client/local_client.h"



Record LocalBrokerConsumerClient::consume(const std::string& topic, uint32_t partition_id, uint64_t offset) {
    return broker_->consume(topic, partition_id, offset);
}

void LocalBrokerConsumerClient::commit_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id, uint64_t offset) {
    broker_->commit_offset(group_id, topic, partition_id, offset);
}

uint64_t LocalBrokerConsumerClient::get_committed_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id) {
    return broker_->get_committed_offset(group_id, topic, partition_id);
}
