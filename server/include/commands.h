#pragma once

#include <grpc++/grpc++.h>

#include "music.h"
#include "yamp.grpc.pb.h"

using grpc::ServerContext;
using yamp::Null;

class CommandsImpl final : public yamp::Server::Service {
public:
	CommandsImpl(Music& c) : music(c) {}

private:
    grpc::Status Play(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Pause(ServerContext* c, const Null*request, Null* reply) override;
   private:
   	Music& music;
};
