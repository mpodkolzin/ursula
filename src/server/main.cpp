#include "httplib.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "partition/partition_writer.h"
#include "broker/broker.h"

void handle_hello(const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World", "text/plain");

}

int main() {
    // Create a server instance
    httplib::Server svr;


    // Define a route handler


    svr.Get("/hello", handle_hello);
    spdlog::info("Broker started at {}", 8080);
    spdlog::warn("Broker started at {}", 8080);
    Broker broker("./data");


    //std::cout << "Server is starting at http://localhost:8080/" << std::endl;

    // Start listening on port 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}