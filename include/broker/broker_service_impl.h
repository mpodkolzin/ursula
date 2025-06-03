#pragma once

#include "broker.grpc.pb.h"
#include "broker.pb.h"
#include "broker/broker.h"
#include "consumer/consumer_group_manager.h"
#include <memory>

class BrokerServiceImpl final : public broker::BrokerService::Service {
public:
    explicit BrokerServiceImpl(
        std::shared_ptr<Broker> broker,
        std::shared_ptr<ConsumerGroupManager> consumer_group_manager);

    grpc::Status Produce(grpc::ServerContext* context,
                         const broker::ProduceRequest* request,
                         broker::ProduceResponse* response) override;

    grpc::Status Consume(grpc::ServerContext* context,
                         const broker::ConsumeRequest* request,
                         broker::ConsumeResponse* response) override;

    grpc::Status CommitOffset(grpc::ServerContext* context,
                              const broker::CommitOffsetRequest* request,
                              broker::Empty* response) override;

    grpc::Status GetCommittedOffset(grpc::ServerContext* context,
                                    const broker::GetCommittedOffsetRequest* request,
                                    broker::CommittedOffsetResponse* response) override;

    grpc::Status ConsumeStream(grpc::ServerContext* context,
                               const broker::ConsumeRequest* request,
                               grpc::ServerWriter<broker::ConsumeResponse>* writer) override;

    grpc::Status Subscribe(grpc::ServerContext* context,
                           const broker::SubscribeRequest* request,
                           broker::Empty* response) override;

private:
    std::shared_ptr<Broker> broker_;
    std::shared_ptr<ConsumerGroupManager> consumer_group_manager_;
};