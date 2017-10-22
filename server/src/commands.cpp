#include <iostream>
#include <string>

#include <fmt/format.h>

#include "commands.h"

using namespace fmt::literals;

void RunServer() {
    std::string server_address("{}:{}"_format("0.0.0.0", "50051"));
    CommandsImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

Status CommandsImpl::SayHello(ServerContext* context,
                              const HelloRequest* request, HelloReply* reply) {
    std::string prefix("Hello ");
    std::cout << request->name() << std::endl;
    reply->set_message(prefix + request->name());
    return Status::OK;
}
