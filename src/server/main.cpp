#include "httplib.h"
#include <iostream>


void handle_hello(const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World", "text/plain");

}

int main() {
    // Create a server instance
    httplib::Server svr;


    // Define a route handler


    svr.Get("/hello", handle_hello);


    std::cout << "Server is starting at http://localhost:8080/" << std::endl;

    // Start listening on port 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}