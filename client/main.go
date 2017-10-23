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

func parseSortCmd(b context.Context, c pb.ServerClient, cmd string) {
	if len(cmd) == 0 {
		log.Fatal("A sub-option is required")
	}

	switch cmd[0] {
	case 'r':
		_, err := c.SortRandom(b, &pb.Null{})
		check(err)
	case 'l':
		_, err := c.SortLLF(b, &pb.Null{})
		check(err)
	default:
		log.Fatal("Invalid command")
	}
}

func parseGetMetadataCmd(b context.Context, c pb.ServerClient, cmd string) {
	if len(cmd) == 0 {
		log.Fatal("A sub-option is required")
	}

	switch cmd[0] {
	case 'a':
		response, err := c.GetArtist(b, &pb.Null{})
		check(err)
		fmt.Println(response.Artist)
	case 't':
		response, err := c.GetTitle(b, &pb.Null{})
		check(err)
		fmt.Println(response.Title)
	case 'f':
		response, err := c.GetFile(b, &pb.Null{})
		check(err)
		fmt.Println(response.File)
	default:
		log.Fatal("Invalid command")
	}
}

func parseCmd(b context.Context, c pb.ServerClient, cmd string) {
	switch cmd[0] {
	case 'P':
		_, err := c.Play(b, &pb.Null{})
		check(err)
	case 'p':
		_, err := c.Pause(b, &pb.Null{})
		check(err)
	case 'n':
		_, err := c.Next(b, &pb.Null{})
		check(err)
	case 'b':
		_, err := c.Back(b, &pb.Null{})
		check(err)
	case 'g':
		parseGetMetadataCmd(b, c, cmd[1:])
	case 's':
		parseSortCmd(b, c, cmd[1:])
	default:
		log.Fatal("Invalid command")
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
	parseCmd(b, c, os.Args[1])
}
