#include <catch2/catch_test_macros.hpp>
#include "broker/broker.h"
#include <filesystem>
#include "record/record.h"
#include "spdlog/spdlog.h"
#include "offset_store/mock_offset_store.h"

TEST_CASE("Broker basic produce and consume") {
    std::string test_dir = "./test_broker_data";
    std::filesystem::remove_all(test_dir);

    auto offset_store = std::make_unique<MockOffsetStore>();
    Broker broker(test_dir, 5, std::move(offset_store));
    uint32_t pid = 0;

    std::vector<uint8_t> msg = {'h', 'e', 'l', 'l', 'o'};
    Record record(RecordType::DATA, msg);
    spdlog::debug("Producing message");
    uint64_t offset = broker.produce("my_topic", "my_key", record);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    spdlog::debug("Produced offset {}", offset);

    spdlog::debug("Consuming message");
    auto result = broker.consume("my_topic", "my_key", offset);
    auto payload = result.payload();
    std::string payload_str(payload.begin(), payload.end());
    spdlog::debug("Payload: {}", payload_str);

    spdlog::debug("Consumed offset _ {}", offset);

    //REQUIRE(result == msg);
    spdlog::debug("checking offset");
    REQUIRE(offset == 0);
    spdlog::debug("checking produced");
    REQUIRE(broker.metrics().produced() == 1);
    spdlog::debug("checking consumed");
    REQUIRE(broker.metrics().consumed() == 1);
    spdlog::debug("removing test dir");
    std::filesystem::remove_all(test_dir);
    spdlog::debug("test dir removed");
}
