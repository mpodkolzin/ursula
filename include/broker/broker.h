#pragma once
#include <string>

class Broker {
public:
    Broker();
    ~Broker();

    void start();
    void stop();
private:
    std::string name_;
    std::string address_;
    std::string port_;
    std::string data_dir_;
};