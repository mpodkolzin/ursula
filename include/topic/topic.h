#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "partition/partition.h"
#include "record/record.h"

class Topic {
public:
    Topic(const std::string& name, const std::string& root_dir, size_t num_partitions);

    uint64_t append(const std::string& key, const Record& record);
    Record read(const std::string& key, uint64_t offset);

private:
    std::string name_;
    std::string root_dir_;
    size_t num_partitions_;

    std::unordered_map<uint32_t, std::unique_ptr<Partition>> partitions_;

    uint32_t hash_to_partition(const std::string& key) const;
    Partition& get_partition(uint32_t pid);
};