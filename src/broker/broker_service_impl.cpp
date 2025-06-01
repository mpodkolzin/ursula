// broker_service_impl.cpp

#include "broker/broker_service_impl.h"
#include "record/record.h"
#include "spdlog/spdlog.h"
#include "broker/broker.h"
#include <memory>

BrokerServiceImpl::BrokerServiceImpl(std::shared_ptr<Broker> broker)
    : broker_(std::move(broker)) {}

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
        Record record = broker_->consume(request->topic(), request->partition_id(), request->offset());
        response->set_payload(record.payload().data(), record.payload().size());
        return grpc::Status::OK;
    } catch (const std::exception& ex) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, ex.what());
    }
}

grpc::Status BrokerServiceImpl::CommitOffset(grpc::ServerContext*,
                                             const broker::CommitOffsetRequest* request,
                                             broker::Empty*) {
    broker_->commit_offset(request->group_id(), request->topic(), request->partition_id(), request->offset());
    return grpc::Status::OK;
}

grpc::Status BrokerServiceImpl::GetCommittedOffset(grpc::ServerContext*,
                                                   const broker::GetCommittedOffsetRequest* request,
                                                   broker::CommittedOffsetResponse* response) {
    uint64_t offset = broker_->get_committed_offset(request->group_id(), request->topic(), request->partition_id());
    response->set_offset(offset);
    return grpc::Status::OK;
}
