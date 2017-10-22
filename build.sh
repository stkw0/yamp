(
protoc -I . protos/yamp.proto --go_out=plugins=grpc:.
mv protos/yamp.pb.go client/src/yamp/yamp.pb.go
cd client  || exit
export GOPATH=$GOPATH:$(pwd)
go build
)&
PID=$!

mkdir -p build
cd build || exit
cmake ..
make -j8

wait $PID

