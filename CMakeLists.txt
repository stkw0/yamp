project(yamp)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/protos/yamp.proto)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})


FILE(GLOB_RECURSE SRC src/*.cpp)

add_executable(yamp
   ${SRC}
   ${PROTO_SRCS}
   ${GRPC_SRCS}
)
target_link_libraries(yamp
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)
