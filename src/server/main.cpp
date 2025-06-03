#define CPPHTTPLIB_THREAD_POOL_COUNT 16

#include "httplib.h"
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>
#include <iostream>
#include "spdlog/spdlog.h"
#include "partition/partition_writer.h"
#include "consumer/consumer_group.h"
#include "broker/broker.h"
#include "record/record.h"
#include "nlohmann/json.hpp"
#include "client/local_client.h"    
#include "broker/broker_service_impl.h"
#include "server/rest_server.h"
#include "server/grpc_server.h"
#include "offset_store/file_offset_store.h"
#include "consumer/consumer_group_manager.h"
#include "client/broker_client.h"

void handle_hello(const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World", "text/plain");

}

int main() {
    spdlog::set_level(spdlog::level::info);
    std::string data_dir = "./broker_data";
    auto offset_store = std::make_shared<FileOffsetStore>(data_dir);
    auto broker = std::make_shared<Broker>(data_dir, 5, std::move(offset_store));
    auto consumer_client = std::make_shared<LocalBrokerConsumerClient>(broker);
    auto consumer_group_manager = std::make_shared<ConsumerGroupManager>(consumer_client, offset_store);
    std::thread rest_server_thread([broker]() {
        run_rest_server(broker);
    });
    std::thread grpc_server_thread([broker, consumer_group_manager]() {
        run_grpc_server(broker, consumer_group_manager);
    });

    rest_server_thread.join();
    grpc_server_thread.join();

    return 0;
}

//int main() {
//    // Create a server instance
//    httplib::Server svr;
//
//
//    // Define a route handler
//
//
//    auto broker = std::make_shared<Broker>("./data", 1);
//    auto client = std::make_shared<LocalBrokerConsumerClient>(broker);
//    ConsumerGroup group("test_group", client);
//    group.subscribe("topic1");
//    svr.Get("/hello", handle_hello);
//
//    svr.Get(R"(/data/(.+))", [&broker](const httplib::Request& req, httplib::Response& res) {
//        std::string data = req.matches[1]; // First capture group
//        std::vector<uint8_t> payload(data.begin(), data.end());
//        spdlog::debug("CONTROLLER Producing to topic='my_topic', key='my_key', data='{}'", data);
//        Record record(RecordType::DATA, payload);
//        auto result = broker->produce("my_topic", "my_key", record);
//        auto client = std::make_shared<LocalBrokerConsumerClient>(broker);
//        res.set_content("Produced " + data, "text/plain");
//    });
//
//    svr.Post("/offset", [&group](const httplib::Request& req, httplib::Response& res) {
//        auto data = nlohmann::json::parse(req.body);
//        std::string key = data.value("key", "");
//        std::string topic = data.value("topic", "");
//        uint64_t offset = data.value("offset", 0);
//        std::optional<Record> rec = group.poll(topic, 0);
//        spdlog::debug("CONTROLLER Consuming from topic='{}', key='{}', offset='{}'", topic, key, offset);
//        //Record rec = broker->consume(topic, key, offset);
//
//        std::string result_str(rec.value().payload().begin(), rec.value().payload().end());
//        nlohmann::json result_json = {
//            {"offset", offset},
//            {"payload", result_str},
//            {"status", "ok"}
//        };
//        res.set_content(result_json.dump(), "application/json");
//    });
//
//    svr.Get(R"(/offset/(\d+))", [&broker](const httplib::Request& req, httplib::Response& res) {
//        std::string offset_str = req.matches[1]; // First capture group
//        int offset = std::stoi(offset_str);
//        try {
//            auto result = broker->consume("my_topic", "my_key", offset);
//            auto result_array = result.payload();
//            std::string result_str(result_array.begin(), result_array.end());
//            nlohmann::json result_json = {
//                {"offset", offset},
//                {"payload", result_str }
//            };
//            res.set_content(result_json.dump(), "application/json");
//        } catch (const std::runtime_error& e) {
//            nlohmann::json error_json = {
//                {"error", e.what()}
//            };
//            res.set_content(error_json.dump(), "application/json");
//        }
//    });
//
//    svr.Post("/data", [&broker](const httplib::Request& req, httplib::Response& res) {
//        try {
//            // Parse incoming JSON
//            auto data = nlohmann::json::parse(req.body);
//
//            // Access value (e.g., {"name": "Max"})
//            std::string key = data.value("key", "");
//            std::string topic = data.value("topic", "");
//            std::string value = data.value("value", "");
//
//            Record record(RecordType::DATA, value);
//            auto result = broker->produce(topic, key, record);
//
//            // Create response JSON
//            nlohmann::json response = {
//                {"message", "Produced " + value + " to topic=" + topic + " with key=" + key},
//                {"status", "ok"}
//            };
//            spdlog::debug("Produced {} to topic={} with key={}", value, topic, key);
//            res.set_content(response.dump(), "application/json");
//        } catch (const std::exception& e) {
//            nlohmann::json error = {
//                {"error", "Invalid JSON"},
//                {"details", e.what()}
//            };
//            res.status = 400;
//            res.set_content(error.dump(), "application/json");
//        }
//    });
//
//
//    spdlog::debug("Broker started at {}", 8080);
//    spdlog::warn("Broker started at {}", 8080);
//
//
//    //std::cout << "Server is starting at http://localhost:8080/" << std::endl;
//
//    // Start listening on port 8080
//    svr.listen("0.0.0.0", 8080);
//
//    return 0;
//}