#pragma once
#include <memory>

class Broker;

//void run_grpc_server(Broker& broker);
void run_grpc_server(std::shared_ptr<Broker> broker);