set(CMAKE_CXX_STANDARD 17)

find_package(spdlog REQUIRED)

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


# clang-format

file(GLOB CLANG_FORMAT_FILE .clang-format)

find_program(CLANG_FORMAT clang-format) 
if(EXISTS ${CLANG_FORMAT})
  add_custom_target(clangformat
        COMMAND clang-format
        -style=file
        -assume-filename=${CLANG_FORMAT_FILE}
        -i
        ${SRC}
  )

  add_dependencies(yampd clangformat)
else()
  message(AUTHOR_WARNING "clang-format is not installed, auto formating will not be performed")
endif()