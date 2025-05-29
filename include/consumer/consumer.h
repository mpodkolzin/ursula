#pragma once

#include <string>
#include <unordered_map>
#include "broker/broker.h"
#include "record/record.h"

class Consumer {
public:
    explicit Consumer(Broker& broker);

    void subscribe(const std::string& topic, uint32_t num_partitions);
    Record poll(const std::string& topic, uint32_t partition);

private:
    Broker& broker_;
    std::unordered_map<std::string, std::unordered_map<uint32_t, uint64_t>> offsets_; // topic -> partition -> offset
};
