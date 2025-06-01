#pragma once

#include <memory>

class Broker;

void run_rest_server(std::shared_ptr<Broker> broker);
