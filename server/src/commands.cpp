#include <iostream>
#include <memory>
#include <string>

#include "commands.h"

grpc::Status CommandsImpl::SayHello(ServerContext* context,
                                    const HelloRequest* request, HelloReply* reply) {
    std::string prefix("Hello ");
    std::cout << request->name() << std::endl;
    reply->set_message(prefix + request->name());
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::Play(ServerContext* context, const Null* request, Null* reply) {
    music.SetStatus(Status::Playing);
    return grpc::Status::OK;
}
