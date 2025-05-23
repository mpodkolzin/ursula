#include <catch2/catch_test_macros.hpp>
#include "broker/broker.h"
#include <filesystem>
#include "record/record.h"
#include "spdlog/spdlog.h"

TEST_CASE("Broker basic produce and consume") {
    std::string test_dir = "./test_broker_data";
    std::filesystem::remove_all(test_dir);

    Broker broker(test_dir);
    PartitionId pid = 0;

    std::vector<uint8_t> msg = {'h', 'e', 'l', 'l', 'o'};
    Record record(RecordType::DATA, msg);
    spdlog::info("Producing message");
    uint64_t offset = broker.produce(pid, record);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    spdlog::info("Produced offset {}", offset);

    spdlog::info("Consuming message");
    auto result = broker.consume(pid, offset);
    auto payload = result.payload();
    std::string payload_str(payload.begin(), payload.end());
    spdlog::info("Payload: {}", payload_str);

    spdlog::info("Consumed offset _ {}", offset);

    //REQUIRE(result == msg);
    spdlog::info("checking offset");
    REQUIRE(offset == 0);
    spdlog::info("checking produced");
    REQUIRE(broker.metrics().produced() == 1);
    spdlog::info("checking consumed");
    REQUIRE(broker.metrics().consumed() == 1);
    spdlog::info("removing test dir");
    std::filesystem::remove_all(test_dir);
    spdlog::info("test dir removed");
}
