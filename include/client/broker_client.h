#pragma once
#include "record/record.h"
#include "broker/broker.h"
class BrokerConsumerClient {
public:
    virtual ~BrokerConsumerClient() = default;

    virtual Record consume(const std::string& topic, uint32_t partition_id, uint64_t offset) = 0;
    virtual void commit_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id, uint64_t offset) = 0;
    virtual uint64_t get_committed_offset(const std::string& group_id, const std::string& topic, uint32_t partition_id) = 0;

};
