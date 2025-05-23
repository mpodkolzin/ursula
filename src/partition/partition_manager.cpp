#include <filesystem>
#include "partition/partition_manager.h"
#include "record/record.h"
PartitionManager::PartitionManager(const std::string& data_dir)
    : data_dir_(data_dir) {}

void PartitionManager::create_partition_if_missing(PartitionId pid) {
    std::string partition_dir = data_dir_ + "/partition_" + std::to_string(pid);
    std::filesystem::create_directories(partition_dir);

    if (!writers_.contains(pid)) {
        writers_[pid] = std::make_unique<PartitionWriter>(partition_dir, 0);
    }

    if (!readers_.contains(pid)) {
        readers_[pid] = std::make_unique<PartitionReader>(partition_dir);
    }
}

uint64_t PartitionManager::append(PartitionId pid, const Record& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    create_partition_if_missing(pid);
    return writers_[pid]->append(record);
}

std::vector<uint8_t> PartitionManager::read(PartitionId pid, uint64_t offset) {
    std::lock_guard<std::mutex> lock(mutex_);
    create_partition_if_missing(pid);
    return readers_[pid]->read(offset);
}

void PartitionManager::flush_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [_, writer] : writers_) {
        writer->flush();
    }
}
