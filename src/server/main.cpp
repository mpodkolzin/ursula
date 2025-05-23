#include "httplib.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "partition/partition_writer.h"
#include "broker/broker.h"
#include "record/record.h"

void handle_hello(const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World", "text/plain");

}

int main() {
    // Create a server instance
    httplib::Server svr;


    // Define a route handler


    Broker broker("./data");
    svr.Get("/hello", handle_hello);

    svr.Get(R"(/data/(.+))", [&broker](const httplib::Request& req, httplib::Response& res) {
        std::string data = req.matches[1]; // First capture group
        std::vector<uint8_t> payload(data.begin(), data.end());
        Record record(RecordType::DATA, payload);
        auto result = broker.produce(0,record);
        res.set_content("Produced", "text/plain");
    });

    svr.Get(R"(/offset/(\d+))", [&broker](const httplib::Request& req, httplib::Response& res) {
        std::string offset_str = req.matches[1]; // First capture group
        int offset = std::stoi(offset_str);
        auto result = broker.consume(0,offset);
        auto result_array = result.payload();
        std::string result_str(result_array.begin(), result_array.end());
        res.set_content(result_str, "text/plain");
    });


    spdlog::info("Broker started at {}", 8080);
    spdlog::warn("Broker started at {}", 8080);


    //std::cout << "Server is starting at http://localhost:8080/" << std::endl;

    // Start listening on port 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}