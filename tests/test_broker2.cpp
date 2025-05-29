#include <catch2/catch_test_macros.hpp>
#include "broker/broker.h"
#include "consumer/consumer_group.h"
#include "client/local_client.h"
#include <filesystem>
#include <thread>
#include <chrono>

TEST_CASE("Broker end-to-end: produce, consume, persist") {
    std::string test_dir = "./test_data_broker";
    std::filesystem::remove_all(test_dir);

    // Step 1: Create broker and produce
    {
        auto broker = std::make_shared<Broker>(test_dir, 1);
        std::string topic = "e2e_topic";
        std::string key = "e2e_key";
        Record record(RecordType::DATA, {'h', 'e', 'l', 'l', 'o'});

        auto client = std::make_shared<LocalBrokerConsumerClient>(broker);

        uint64_t offset = broker->produce(topic, key, record);
        REQUIRE(offset == 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // allow flush

        ConsumerGroup group("test_group", client);
        group.subscribe(topic);
        std::optional<Record> out = group.poll(topic, 1);
        REQUIRE(out.has_value());
        REQUIRE(out.value().payload() == record.payload());
    }

    // Step 2: Simulate restart
    {
        Record record(RecordType::DATA, {'h', 'e', 'l', 'l', 'o'});
        std::string topic = "e2e_topic";
        auto broker = std::make_shared<Broker>(test_dir, 1);
        auto client = std::make_shared<LocalBrokerConsumerClient>(broker);
        ConsumerGroup group("test_group", client);
        group.subscribe(topic);

        std::optional<Record> out = group.poll(topic, 1);
        REQUIRE(out.has_value());
        REQUIRE(out.value().payload() == record.payload());
    }

    std::filesystem::remove_all(test_dir);
}
