#include <iostream>
#include <memory>
#include <string>

#include "commands.h"

grpc::Status CommandsImpl::Play(ServerContext* c, const Null* request, Null* reply) {
    music.SetStatus(Status::Playing);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::Pause(ServerContext* c, const Null* request, Null* reply) {
    if(music.GetStatus() != Status::Playing) {
        music.SetStatus(Status::Playing);
    } else {
        music.SetStatus(Status::Paused);
    }
    return grpc::Status::OK;
}
