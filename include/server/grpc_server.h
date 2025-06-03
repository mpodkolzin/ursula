#pragma once
#include <memory>


class Broker;
class ConsumerGroupManager;

void run_grpc_server(std::shared_ptr<Broker> broker, std::shared_ptr<ConsumerGroupManager> consumer_group_manager);
