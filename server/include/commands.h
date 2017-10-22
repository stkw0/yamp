#pragma once

#include <grpc++/grpc++.h>

#include "yamp.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using yamp::HelloReply;
using yamp::HelloRequest;

void RunServer();

class CommandsImpl final : public yamp::Server::Service {
    Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* reply) override;
};
