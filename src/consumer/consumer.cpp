#include <string>
#include <unordered_map>
#include <string>
#include <unordered_map>
#include "broker/broker.h"
#include "record/record.h"
#include "consumer/consumer.h"

Consumer::Consumer(Broker& broker) : broker_(broker) {}

void Consumer::subscribe(const std::string& topic, uint32_t num_partitions) {
    for (uint32_t pid = 0; pid < num_partitions; ++pid) {
        offsets_[topic][pid] = 0;
    }
}

Record Consumer::poll(const std::string& topic, uint32_t partition) {
    uint64_t offset = offsets_[topic][partition];
    Record record = broker_.consume(topic, partition, offset);
    offsets_[topic][partition]++;
    return record;
}
