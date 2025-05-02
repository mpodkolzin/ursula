#pragma once

#include <string>
#include <vector>
#include <memory>
#include "partition/partition_manager.h"
#include "metrics/metrics_collector.h"

class Broker {
public:
    explicit Broker(const std::string& data_dir);

    uint64_t produce(PartitionId pid, const std::vector<uint8_t>& message);
    //std::vector<uint8_t> consume(PartitionId pid, uint64_t offset);
    MetricsCollector& metrics() const;

    void flush();

private:
    std::unique_ptr<PartitionManager> partition_manager_;
    std::unique_ptr<MetricsCollector> metrics_collector_;
};
