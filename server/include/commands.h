#pragma once

#include <grpc++/grpc++.h>

#include "music.h"
#include "yamp.grpc.pb.h"

using grpc::ServerContext;
using yamp::Null;
using yamp::Artist;
using yamp::Title;
using yamp::File;

class CommandsImpl final : public yamp::Server::Service {
public:
	CommandsImpl(Music& c) : music(c) {}

private:
    grpc::Status Play(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Pause(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Next(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status Back(ServerContext* c, const Null* request, Null* reply) override;

    // Metadata commands
    grpc::Status GetArtist(ServerContext* c, const Null* request, Artist* reply) override;
    grpc::Status GetTitle(ServerContext* c, const Null* request, Title* reply) override;
    grpc::Status GetFile(ServerContext* c, const Null* request, File* reply) override;

    // Sort commands
    grpc::Status SortRandom(ServerContext* c, const Null* request, Null* reply) override;
    grpc::Status SortLLF(ServerContext* c, const Null* request, Null* reply) override;

   private:
   	Music& music;
};
