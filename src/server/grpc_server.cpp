#include "server/grpc_server.h"
#include "broker/broker.h"
#include "broker/broker_service_impl.h"
#include "consumer/consumer_group_manager.h"
#include <grpcpp/grpcpp.h>
#include <memory>

void run_grpc_server(std::shared_ptr<Broker> broker, std::shared_ptr<ConsumerGroupManager> consumer_group_manager) {

    std::string server_address("0.0.0.0:50051");
    BrokerServiceImpl service(broker, consumer_group_manager);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Broker gRPC server listening on " << server_address << std::endl;
    server->Wait();
}