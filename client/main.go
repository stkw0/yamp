package main

import (
	"log"
	"os"

	"context"
	"google.golang.org/grpc"
	pb "yamp"
)

const (
	address = "localhost:6600"
)

func check(i interface{}, err error) {
	if err != nil {
		log.Fatalf("could not greet: %v", err)
	}
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("An argument is required")
	}

	// Set up a connection to the server.
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := pb.NewServerClient(conn)

	b := context.Background()

	switch os.Args[1] {
	case "P":
		check(c.Play(b, &pb.Null{}))
	case "p":
		c.Pause(b, &pb.Null{})
	}
}
