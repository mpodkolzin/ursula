#include "broker/broker.h"
#include "partition/partition_manager.h"
#include "metrics/metrics_collector.h"
#include "record/record.h"

Broker::Broker(const std::string& data_dir) {
    partition_manager_ = std::make_unique<PartitionManager>(data_dir);
    metrics_collector_ = std::make_unique<MetricsCollector>();
}

uint64_t Broker::produce(PartitionId pid, const Record& record) {
    metrics_collector_->increment_produced();
    return partition_manager_->append(pid, record);
}

Record Broker::consume(PartitionId pid, uint64_t offset) {
    metrics_collector_->increment_consumed();
    return partition_manager_->read(pid, offset);
}

void Broker::flush() {
    partition_manager_->flush_all();
}

MetricsCollector& Broker::metrics() const {
    return *metrics_collector_;
}