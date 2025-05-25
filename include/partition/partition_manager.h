//#pragma once
//
//#include <unordered_map>
//#include <memory>
//#include <mutex>
//#include "partition/partition_writer.h"
//#include "partition/partition_reader.h"
//#include "record/record.h"
//
//
//class PartitionManager {
//public:
//
//    PartitionManager(const std::string& root_dir, size_t default_partitions_per_topic);
//
//    PartitionWriter& get_partition_writer(const std::string& topic, PartitionId pid);
//    PartitionReader& get_partition_reader(const std::string& topic, PartitionId pid);
//
//    uint64_t append(const std::string& topic, PartitionId pid, const Record& record);
//    Record read(const std::string& topic, PartitionId pid, uint64_t offset);
//
//    PartitionId hash_to_partition(const std::string& topic, const std::string& key) const;
//
//    void create_partition(const std::string& topic, PartitionId pid); // explicit creation hook
//
//private:
//    std::string data_dir_;
//    std::mutex mutex_;
//    std::unordered_map<PartitionId, std::unique_ptr<PartitionWriter>> writers_;
//    std::unordered_map<PartitionId, std::unique_ptr<PartitionReader>> readers_;
//
//    size_t default_partitions_;
//    
//    PartitionId hash_to_partition(const std::string& key, size_t num_partitions) const;
//    void maybe_init_topic(const std::string& topic);
//
//};
//