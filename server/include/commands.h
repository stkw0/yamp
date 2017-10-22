#pragma once

#include <grpc++/grpc++.h>

#include "music.h"
#include "yamp.grpc.pb.h"

using grpc::ServerContext;
using yamp::HelloReply;
using yamp::HelloRequest;
using yamp::Null;

class CommandsImpl final : public yamp::Server::Service {
public:
	CommandsImpl(Music& c) : music(c) {}

private:
    grpc::Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* reply) override;

    grpc::Status Play(ServerContext* context, const Null* request, Null* reply) override;
   private:
   	Music& music;
};
