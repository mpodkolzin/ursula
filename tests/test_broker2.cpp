#include <catch2/catch_test_macros.hpp>
#include "broker/broker.h"
#include <filesystem>
#include "record/record.h"
#include "spdlog/spdlog.h"


TEST_CASE("Broker end-to-end: produce, consume, persist") {
    std::string test_dir = "./test_data_broker";
    std::filesystem::remove_all(test_dir);

    // Step 1: Create broker and produce
    {
        Broker broker(test_dir);
        std::string topic = "e2e_topic";
        std::string key = "e2e_key";
        Record record(RecordType::DATA, {'h', 'e', 'l', 'l', 'o'});

        uint64_t offset = broker.produce(topic, key, record);
        REQUIRE(offset == 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // allow flush

        Record out = broker.consume(topic, key, offset);
        REQUIRE(out.payload() == record.payload());
    }

    // Step 2: Simulate restart
    {
        Broker broker(test_dir);
        std::string topic = "e2e_topic";
        std::string key = "e2e_key";

        Record out = broker.consume(topic, key, 0);
        REQUIRE(out.payload() == std::vector<uint8_t>{'h', 'e', 'l', 'l', 'o'});
    }

    std::filesystem::remove_all(test_dir);
}
