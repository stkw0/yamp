package main

import (
	"context"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/user"
	"strconv"
	"strings"

	"google.golang.org/grpc"
	"gopkg.in/yaml.v2"
	pb "yamp"
)

const (
	config_file = "~/.config/yamp/config"
)

type ConfStruct struct {
	Pid_file     string `yaml:"pid_file"`
	Music_folder string `yaml:"music_folder"`
	Log_file     string `yaml:"log_file"`
	Log_level    int    `yaml:"log_level"`
	Port_number  int    `yaml:"port_number"`
	Bind_address string `yaml:"bind_address"`
}

var conf ConfStruct

func check(err error) {
	if err != nil {
		log.Fatalf("Error: %v", err)
	}
}

func parseVolumeSetCmd(b context.Context, c pb.ServerClient, cmd string) {
	if len(cmd) == 0 {
		log.Fatal("A sub-option is required")
	}

	switch cmd[0] {
	case '-':
		f, err := strconv.ParseFloat(os.Args[2][1:], 32)
		check(err)
		_, err = c.VolumeSet(b, &pb.Volume{Volume: float32(f), ActionType: pb.Volume_DECREASE})
		check(err)
	case '+':
		f, err := strconv.ParseFloat(os.Args[2][1:], 32)
		check(err)
		_, err = c.VolumeSet(b, &pb.Volume{Volume: float32(f), ActionType: pb.Volume_INCREASE})
		check(err)
	default:
		f, err := strconv.ParseFloat(os.Args[2], 32)
		check(err)
		_, err = c.VolumeSet(b, &pb.Volume{Volume: float32(f), ActionType: pb.Volume_SET})
		check(err)
	}
}

func parseVolumeCmd(b context.Context, c pb.ServerClient, cmd string) {
	if len(cmd) == 0 {
		log.Fatal("A sub-option is required")
	}

	switch cmd[0] {
	case 'g':
		response, err := c.VolumeGet(b, &pb.Null{})
		check(err)
		fmt.Println(response.Volume)
	case 's':
		parseVolumeSetCmd(b, c, os.Args[2])
	default:
		log.Fatal("Invalid command")
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

func parseInfoCmd(b context.Context, c pb.ServerClient, cmd string) {
	if len(cmd) == 0 {
		log.Fatal("A sub-option is required")
	}

	switch cmd[0] {
	case 's':
		response, err := c.GetInfoStatus(b, &pb.Null{})
		check(err)
		fmt.Println(response.Status)
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
	case 'v':
		parseVolumeCmd(b, c, cmd[1:])
	case 'i':
		parseInfoCmd(b, c, cmd[1:])
	default:
		log.Fatal("Invalid command")
	}
}

func Expand(path string) string {
	usr, err := user.Current()
	check(err)

	return strings.Replace(path, "~", usr.HomeDir, 1)
}

func main() {
	data, err := ioutil.ReadFile(Expand(config_file))
	if err != nil {
		log.Fatalf("erro: %v", err)
	}

	err = yaml.Unmarshal([]byte(data), &conf)
	if err != nil {
		log.Fatalf("error: %v", err)
	}

	if len(os.Args) < 2 {
		log.Fatal("An argument is required")
	}

	// Set up a connection to the server.
	address := conf.Bind_address + ":" + strconv.Itoa(conf.Port_number)
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()

	c := pb.NewServerClient(conn)
	b := context.Background()
	parseCmd(b, c, os.Args[1])
}
