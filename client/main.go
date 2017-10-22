package main

import (
	"log"

	"golang.org/x/net/context"
	"google.golang.org/grpc"
	pb "yamp"
)

const (
	address = "localhost:6600"
)

func main() {
	// Set up a connection to the server.
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := pb.NewServerClient(conn)

	// Contact the server and print out its response.
	_, err = c.Play(context.Background(), &pb.Null{})
	if err != nil {
		log.Fatalf("could not greet: %v", err)
	}
}
