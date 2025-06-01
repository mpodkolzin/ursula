#include "server/rest_server.h"
#include "broker/broker.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

void run_rest_server(std::shared_ptr<Broker> broker) {
    httplib::Server svr;

    svr.Post("/produce", [&](const httplib::Request& req, httplib::Response& res) {
        auto json = nlohmann::json::parse(req.body);
        std::string topic = json["topic"];
        std::string key = json["key"];
        std::string payload_str = json["payload"];

        std::vector<uint8_t> payload(payload_str.begin(), payload_str.end());
        Record record(RecordType::DATA, payload);
        uint64_t offset = broker->produce(topic, key, record);

        res.set_content(nlohmann::json{{"offset", offset}}.dump(), "application/json");
    });

    svr.Get("/consume", [&](const httplib::Request& req, httplib::Response& res) {
        auto topic = req.get_param_value("topic");
        auto key = req.get_param_value("key");
        uint64_t offset = std::stoull(req.get_param_value("offset"));

        Record record = broker->consume(topic, key, offset);
        std::string payload_str(record.payload().begin(), record.payload().end());

        res.set_content(nlohmann::json{
            {"payload", payload_str}
        }.dump(), "application/json");
    });

    std::cout << "REST server listening on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}
