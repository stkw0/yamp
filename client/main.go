package main

import (
	"fmt"
	"log"
	"os"

	"context"
	"google.golang.org/grpc"
	pb "yamp"
)

const (
	address = "localhost:6600"
)

func check(err error) {
	if err != nil {
		log.Fatalf("Error: %v", err)
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
		_, err = c.Play(b, &pb.Null{})
		check(err)
	case "p":
		_, err = c.Pause(b, &pb.Null{})
		check(err)
	case "a":
		response, _ := c.GetArtist(b, &pb.Null{})
		check(err)
		fmt.Println(response.Artist)
	}
}
