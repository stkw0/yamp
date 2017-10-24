protobuf_generate_go(PROTO_SRCS ${PROTO_SRC_DIR} ${PROTOS})

file(GLOB_RECURSE SRC *.go)

if(":$ENV{GOPATH}" STREQUAL ":")
	set(GO_PATH ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(GO_PATH ${CMAKE_CURRENT_BINARY_DIR}:$ENV{GOPATH})
endif()

# It sucks
add_custom_command(
  OUTPUT go_get
  COMMAND bash
  ARGS "-c" "export GOPATH=${GO_PATH}; go get -v google.golang.org/grpc; touch go_get"
  DEPENDS go_protobuf
  VERBATIM )


add_custom_command(
  OUTPUT yampc
  COMMAND bash
  ARGS "-c" "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/src/yamp; cp ${PROTO_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/yamp; export GOPATH=${GO_PATH}; go build -o yampc ${SRC}"
  DEPENDS go_protobuf go_get
  VERBATIM )

add_custom_target(yampc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/yampc)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/yampc DESTINATION bin)
