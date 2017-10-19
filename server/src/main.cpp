#include <string>

#include <grpc++/grpc++.h>

#include "yamp.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using yamp::HelloRequest;
using yamp::HelloReply;

class GreeterServiceImpl final : public yamp::Server::Service {
   Status SayHello(ServerContext* context, const HelloRequest* request, HelloReply* reply) override {
      std::string prefix("Hello ");
      reply->set_message(prefix + request->name());
      return Status::OK;
   }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main() {
   RunServer();
}
