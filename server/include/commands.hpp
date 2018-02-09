#pragma once

#include <grpc++/grpc++.h>

#include "music.hpp"
#include "yamp.grpc.pb.h"

using grpc::ServerContext;
using yamp::Null;
using yamp::Artist;
using yamp::Title;
using yamp::File;
using yamp::Volume;
using yamp::Info;
using yamp::Offset;

class CommandsImpl final : public yamp::Server::Service {
public:
	CommandsImpl(Music& c) : music(c) {}

private:
    grpc::Status Play(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Pause(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Next(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Back(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Clear(ServerContext* c, const Null* request, Null* reply) override;

    // Metadata commands
    grpc::Status GetArtist(ServerContext* c, const Null* request, Artist* reply) override;
    grpc::Status GetTitle(ServerContext* c, const Null* request, Title* reply) override;
    grpc::Status GetFile(ServerContext* c, const Null* request, File* reply) override;

    // Sort commands
    grpc::Status SortRandom(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status SortLLF(ServerContext* c, const Null* request, Null* reply) override;

    // Volume commands
    grpc::Status VolumeGet(ServerContext* c, const Null* request, Volume* reply) override;
    grpc::Status VolumeSet(ServerContext* c, const Volume* request, Null* reply) override;

    // Filter commands
    grpc::Status FilterArtist(ServerContext* c, const Artist* request, Null* reply) override;

    // Add commands
    grpc::Status AddFile(ServerContext* c, const File* request,Null* reply) override;
    grpc::Status AddFolder(ServerContext* c, const File* request,Null* reply) override;

    // Time offset commands
    grpc::Status GetRemainingTime(ServerContext* c, const Null* request, Offset* reply) override;
    grpc::Status SetOffsetTime(ServerContext* c, const Offset* request, Null* reply) override;

    grpc::Status GetInfoStatus(ServerContext* c, const Null* request, Info* reply) override;

   private:
   	Music& music;
};
