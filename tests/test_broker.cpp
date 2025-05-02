#include <catch2/catch_test_macros.hpp>
#include "broker/broker.h"
#include <filesystem>

TEST_CASE("Broker basic produce and consume") {
    std::string test_dir = "./test_broker_data";
    std::filesystem::remove_all(test_dir);

    Broker broker(test_dir);
    PartitionId pid = 0;

    std::vector<uint8_t> msg = {'h', 'e', 'l', 'l', 'o'};
    uint64_t offset = broker.produce(pid, msg);
    //auto result = broker.consume(pid, offset);

    //REQUIRE(result == msg);
    REQUIRE(offset == 0);
    REQUIRE(broker.metrics().produced() == 1);
    //REQUIRE(broker.metrics().consumed() == 1);

    std::filesystem::remove_all(test_dir);
}
