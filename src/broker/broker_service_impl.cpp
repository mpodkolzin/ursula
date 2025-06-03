// broker_service_impl.cpp

#include "broker/broker_service_impl.h"
#include "record/record.h"
#include "spdlog/spdlog.h"
#include "broker/broker.h"
#include <memory>
#include "consumer/consumer_group_manager.h"
#include <spdlog/spdlog.h>

BrokerServiceImpl::BrokerServiceImpl(
    std::shared_ptr<Broker> broker,
    std::shared_ptr<ConsumerGroupManager> consumer_group_manager)
    : broker_(std::move(broker)),
      consumer_group_manager_(std::move(consumer_group_manager)) {}

grpc::Status BrokerServiceImpl::Produce(grpc::ServerContext*,
                                        const broker::ProduceRequest* request,
                                        broker::ProduceResponse* response) {
    Record record(RecordType::DATA, std::vector<uint8_t>(
        request->payload().begin(), request->payload().end()));

    uint64_t offset = broker_->produce(request->topic(), request->key(), record);
    response->set_offset(offset);
    return grpc::Status::OK;
}

grpc::Status BrokerServiceImpl::Consume(grpc::ServerContext*,
                                        const broker::ConsumeRequest* request,
                                        broker::ConsumeResponse* response) {
    try {
        auto group = consumer_group_manager_->get_or_create(request->group_id());
        auto record = group->poll(request->topic(), request->partition_id());
        //Record record = broker_->consume(request->topic(), request->partition_id(), request->offset());
        response->set_payload(record.value().payload().data(), record.value().payload().size());
        return grpc::Status::OK;
    } catch (const std::exception& ex) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, ex.what());
    }
}


grpc::Status BrokerServiceImpl::CommitOffset(
    grpc::ServerContext* context,
    const broker::CommitOffsetRequest* request,
    broker::Empty* response) 
{
    spdlog::info("CommitOffset: group_id={}, topic={}, partition_id={}", request->group_id(), request->topic(), request->partition_id());
    auto group_id = request->group_id();
    auto topic = request->topic();
    auto partition_id = request->partition_id();

    auto group = consumer_group_manager_->get_or_create(group_id);

    group->commit(topic, partition_id);
    return grpc::Status::OK;
}

grpc::Status BrokerServiceImpl::Subscribe(
    grpc::ServerContext* context,
    const broker::SubscribeRequest* request,
    broker::Empty* response) {
  
    auto group = consumer_group_manager_->get_or_create(request->group_id());

    group->subscribe(request->topic());
    return grpc::Status::OK;
}


grpc::Status BrokerServiceImpl::GetCommittedOffset(grpc::ServerContext*,
                                                   const broker::GetCommittedOffsetRequest* request,
                                                   broker::CommittedOffsetResponse* response) {
    uint64_t offset = broker_->get_committed_offset(request->group_id(), request->topic(), request->partition_id());
    response->set_offset(offset);
    return grpc::Status::OK;
}

grpc::Status BrokerServiceImpl::ConsumeStream(grpc::ServerContext* context,
                                              const broker::ConsumeRequest* request,
                                              grpc::ServerWriter<broker::ConsumeResponse>* writer) {

    uint64_t offset = request->offset();
    const std::string& topic = request->topic();
    uint32_t partition_id = request->partition_id();

    //uint64_t offset = broker_->get_committed_offset(group_id, topic, partition_id);

    while (!context->IsCancelled()) {
        Record record = broker_->consume(topic, partition_id, offset);

        broker::ConsumeResponse response;
        response.set_payload(record.payload().data(), record.payload().size());

        if (!writer->Write(response)) break;

        // Update offset and commit it
        ++offset;
        //broker_->commit_offset(group_id, topic, partition_id, offset);
    }

    return grpc::Status::OK;
}
