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

grpc::Status CommandsImpl::GetArtist(ServerContext* c, const Null* request, Artist* reply) {
    auto artist = music.GetCurrent().GetArtist();
    reply->set_artist(artist);
    return grpc::Status::OK;
}
