#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "partition/partition.h"
#include "record/record.h"
#include "producer/partition_worker.h"

class Topic {
public:
    Topic(const std::string& name, const std::string& root_dir, size_t num_partitions);
    ~Topic();

    uint64_t append(const std::string& key, const Record& record);
    Record read(const std::string& key, uint64_t offset);
    Record read(uint32_t partition_id, uint64_t offset);

    //alternative producer implementation
    std::future<uint64_t> append_async(const std::string& key, const Record& record);
    void start_workers();
    void stop_workers();

private:
    std::string name_;
    std::string root_dir_;
    size_t num_partitions_;

    std::unordered_map<uint32_t, std::unique_ptr<Partition>> partitions_;
    std::unordered_map<uint32_t, std::unique_ptr<PartitionWorker>> workers_;

    uint32_t hash_to_partition(const std::string& key) const;
    Partition& get_partition(uint32_t pid);
};