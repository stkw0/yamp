package main

import (
	"log"

	"google.golang.org/grpc"
   "yamp"
)

const (
	address     = "localhost:50051"
)

func main() {
	// Set up a connection to the server.
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()

   c := yamp.NewServerClient(conn)

}
