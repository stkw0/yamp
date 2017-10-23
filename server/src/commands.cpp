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

grpc::Status CommandsImpl::Next(ServerContext* c, const Null* request, Null* reply) {
    music.SetStatus(Status::Forwarding);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::Back(ServerContext* c, const Null* request, Null* reply) {
    music.SetStatus(Status::Backing);
    return grpc::Status::OK;
}

// Metadata commands

grpc::Status CommandsImpl::GetArtist(ServerContext* c, const Null* request, Artist* reply) {
    auto artist = music.GetCurrent().GetArtist();
    reply->set_artist(artist);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::GetTitle(ServerContext* c, const Null* request, Title* reply) {
    auto title = music.GetCurrent().GetTitle();
    reply->set_title(title);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::GetFile(ServerContext* c, const Null* request, File* reply) {
    auto file = music.GetCurrent().GetFile();
    reply->set_file(file);
    return grpc::Status::OK;
}

// Sort commands

grpc::Status CommandsImpl::SortRandom(ServerContext* c, const Null* request, Null* reply) {
    music.GetList().Sort(Order::RANDOM);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::SortLLF(ServerContext* c, const Null* request, Null* reply) {
    music.GetList().Sort(Order::LLF);
    return grpc::Status::OK;
}

// Volume commands

grpc::Status CommandsImpl::VolumeGet(ServerContext* c, const Null* request, Volume* reply) {
    reply->set_volume(music.GetVolume());
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::VolumeSet(ServerContext* c, const Volume* request, Null* reply) {
    auto v = request->volume();
    auto t = request->action_type();
    switch(t) {
    // TODO
    case yamp::Volume_Type_GET:
        break;
    }
    std::cout << t << std::endl;
    return grpc::Status::OK;
}
