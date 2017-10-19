set(CMAKE_CXX_STANDARD 17)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

FILE(GLOB_RECURSE SRC src/*.cpp)

add_executable(yampd
   ${SRC}
   ${PROTO_SRCS}
   ${GRPC_SRCS}
)
target_link_libraries(yampd
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)
