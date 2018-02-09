#include <iostream>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include "commands.hpp"

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

grpc::Status CommandsImpl::Clear(ServerContext* c, const Null* request, Null* reply) {
    music.SetStatus(Status::Stoped);
    music.GetList().GetSongList().clear();
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

    float new_volume;
    switch(t) {
    case yamp::Volume_Type_SET:
        new_volume = v;
        break;
    case yamp::Volume_Type_INCREASE:
        new_volume = music.GetVolume() + v;
        break;
    case yamp::Volume_Type_DECREASE:
        new_volume = music.GetVolume() - v;
        break;
    default:
        spdlog::get("global")->error(
            "Invalid action type on VolumeSet handler: {}", t);
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                            "Invalid action type for VolumeSet");
    }

    if(new_volume > 100.0) new_volume = 100;
    if(new_volume < 0) new_volume = 0;
    music.SetVolume(new_volume);

    return grpc::Status::OK;
}

// Filter commands

grpc::Status CommandsImpl::FilterArtist(ServerContext* c, const Artist* request, Null* reply) {
    auto artist = request->artist();
    auto tmp = music.GetStatus();
    music.SetStatus(Status::Stoped);
    music.GetList().FilterArtist(artist);
    music.SetStatus(tmp);

    return grpc::Status::OK;
}

// Add Commands

grpc::Status CommandsImpl::AddFile(ServerContext* c, const File* request, Null* reply) {
    auto file = request->file();
    music.GetList().LoadFile(file);

    return grpc::Status::OK;
}

grpc::Status CommandsImpl::AddFolder(ServerContext* c, const File* request, Null* reply) {
    auto file = request->file();
    music.GetList().LoadDir(file);

    return grpc::Status::OK;
}

// Time offset commands
grpc::Status CommandsImpl::GetRemainingTime(ServerContext* c,
                                            const Null* request, Offset* reply) {
    auto remaining = music.GetRemainingMilliseconds();
    reply->set_offset(remaining);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::SetOffsetTime(ServerContext* c,
                                         const Offset* request, Null* reply) {
    auto offset = request->offset();
    music.SetPlayingOffset(offset);
    return grpc::Status::OK;
}

grpc::Status CommandsImpl::GetInfoStatus(ServerContext* c, const Null* request, Info* reply) {
    reply->set_status(static_cast<yamp::Info_Status>(music.GetStatus()));
    return grpc::Status::OK;
}
