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
	config_file         = "~/.config/yamp/yampc.yml"
	valid_main_commands = `
		q		Quit
		P		Play
		p		Pause
		n		Next
		b		Back
		C		Clear
		g 		Get some metadata of the current song
		s 		Sort commands
		v 		Volume commands
		i 		Info commands
		f 		Filter commands
		a 		Add commands
		t 		Time commands
		L		Playlist commands`
)

type ConfStruct struct {
	Server_ip   string `yaml:"server_ip"`
	Log_file    string `yaml:"log_file"`
	Port_number string `yaml:"port_number"`
}

var conf ConfStruct

func check(err error) {
	if err != nil {
		log.Fatalf("Error: %v", err)
	}
}

func canOpen(path string) bool {
	_, err := os.Stat(path)
	if err == nil {
		return true
	}
	return false
}

func getFullPath(rel_path string) string {
	if rel_path[0] == '/' {
		return rel_path
	}

	path := "/" + rel_path
	if path == "/." || !canOpen(path) {
		pwd, err := os.Getwd()
		check(err)
		path = pwd + "/" + path
	}

	return path
}

func addCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		f 		Add a single file
		F 		Add a folder
		r 		Play a single file (clears the playlist and add that song)
		R 		Play songs inside a folder (same as above but with a folder)`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	path := getFullPath(os.Args[2])

	switch cmd[0] {
	case 'f':
		_, err := c.AddFile(b, &pb.File{File: path})
		check(err)
	case 'F':
		_, err := c.AddFolder(b, &pb.File{File: path})
		check(err)
	case 'r':
		_, err := c.Clear(b, &pb.Null{})
		check(err)
		_, err = c.AddFile(b, &pb.File{File: path})
		check(err)
		_, err = c.Play(b, &pb.Null{})
		check(err)
	case 'R':
		_, err := c.Clear(b, &pb.Null{})
		check(err)
		_, err = c.AddFolder(b, &pb.File{File: path})
		check(err)
		_, err = c.Play(b, &pb.Null{})
		check(err)
	default:
		showHelp(valid_commands)
	}
}

func parseFilterCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		a 		Filter by artist`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	switch cmd[0] {
	case 'a':
		_, err := c.FilterArtist(b, &pb.Artist{Artist: os.Args[2]})
		check(err)
	default:
		return
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
	valid_commands := `
		g 		Get volume
		s 		Set volume`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	switch cmd[0] {
	case 'g':
		response, err := c.VolumeGet(b, &pb.Null{})
		check(err)
		fmt.Println(response.Volume)
	case 's':
		parseVolumeSetCmd(b, c, os.Args[2])
	default:
		showHelp(valid_commands)
	}
}

func parseSortCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		r 		Random sort
		l		LLF sort
		m 		MLF sort`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	switch cmd[0] {
	case 'r':
		_, err := c.SortRandom(b, &pb.Null{})
		check(err)
	case 'l':
		_, err := c.SortLLF(b, &pb.Null{})
		check(err)
	case 'm':
		_, err := c.SortMLF(b, &pb.Null{})
		check(err)
	default:
		showHelp(valid_commands)
	}
}

func parseGetMetadataCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		a		Get artist
		t 		Get title
		f 		Get File`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
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
		showHelp(valid_commands)
	}
}

func parseInfoCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		s 		Get status`
	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	switch cmd[0] {
	case 's':
		response, err := c.GetInfoStatus(b, &pb.Null{})
		check(err)
		fmt.Println(response.Status)
	default:
		showHelp(valid_commands)
	}
}

func parseTimeCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		g		Get remaining time
		s 		Set offset
		`
	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	switch cmd[0] {
	case 'g':
		response, err := c.GetRemainingTime(b, &pb.Null{})
		check(err)
		fmt.Println(response.Offset)
	case 's':
		f, err := strconv.ParseFloat(os.Args[2], 32)
		check(err)
		_, err = c.SetOffsetTime(b, &pb.Offset{Offset: float32(f)})
		check(err)
	default:
		showHelp(valid_commands)
	}
}

func parsePlaylistCmd(b context.Context, c pb.ServerClient, cmd string) {
	valid_commands := `
		l		Load playlist
		s 		Save playlist
		`

	if len(cmd) == 0 {
		showHelp(valid_commands)
		return
	}

	path := getFullPath(os.Args[2])

	switch cmd[0] {
	case 'l':
		_, err := c.LoadPlaylist(b, &pb.File{File: path})
		check(err)
	case 's':
		_, err := c.SavePlaylist(b, &pb.File{File: path})
		check(err)
	default:
		showHelp(valid_commands)
	}
}

func parseCmd(b context.Context, c pb.ServerClient, cmd string) {
	switch cmd[0] {
	case 'q':
		_, err := c.Exit(b, &pb.Null{})
		check(err)
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
	case 'C':
		_, err := c.Clear(b, &pb.Null{})
		check(err)
	case 'g':
		parseGetMetadataCmd(b, c, cmd[1:])
	case 's':
		parseSortCmd(b, c, cmd[1:])
	case 'v':
		parseVolumeCmd(b, c, cmd[1:])
	case 'i':
		parseInfoCmd(b, c, cmd[1:])
	case 'f':
		parseFilterCmd(b, c, cmd[1:])
	case 'a':
		addCmd(b, c, cmd[1:])
	case 't':
		parseTimeCmd(b, c, cmd[1:])
	case 'L':
		parsePlaylistCmd(b, c, cmd[1:])
	default:
		showHelp(valid_main_commands)
	}
}

func showHelp(valid_commands string) {
	fmt.Println("Invalid or no command")
	fmt.Println("Valid comands are")
	fmt.Println(valid_commands)
}

func Expand(path string) string {
	usr, err := user.Current()
	check(err)

	return strings.Replace(path, "~", usr.HomeDir, 1)
}

func main() {
	data, err := ioutil.ReadFile(Expand(config_file))
	check(err)
	err = yaml.Unmarshal([]byte(data), &conf)
	check(err)

	if len(os.Args) < 2 {
		showHelp(valid_main_commands)
		return
	}

	// Set up a connection to the server.
	address := conf.Server_ip + ":" + conf.Port_number
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()

	c := pb.NewServerClient(conn)
	b := context.Background()
	parseCmd(b, c, os.Args[1])
}
