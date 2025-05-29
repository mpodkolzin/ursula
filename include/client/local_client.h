#pragma once
#include "record/record.h"
#include "client/broker_client.h"
class LocalBrokerConsumerClient: public BrokerConsumerClient {
public:
    LocalBrokerConsumerClient(std::shared_ptr<Broker> broker)
    : broker_(std::move(broker)) {}

    Record consume(const std::string& topic, uint32_t partition_id, uint64_t offset) override;

    void commit_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id, uint64_t offset) override;
    uint64_t get_committed_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id) override;
private:
    std::shared_ptr<Broker> broker_;
};
