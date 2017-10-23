protobuf_generate_go(PROTO_SRCS ${PROTO_SRC_DIR} ${PROTOS})

file(GLOB_RECURSE SRC *.go)

add_custom_command(
  OUTPUT yampc
  COMMAND bash
  ARGS "-c" "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/src/yamp; cp ${PROTO_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/yamp; GOPATH+=:${CMAKE_CURRENT_BINARY_DIR}; export GOPATH; go build -o yampc ${SRC}"
  DEPENDS go_protobuf
  VERBATIM )

add_custom_target(yampc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/yampc)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/yampc DESTINATION bin)
