set(CMAKE_CXX_STANDARD 17)

find_package(GRPC REQUIRED)
find_package(SFML 2 REQUIRED SYSTEM AUDIO)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
# TODO: Next upstream version will support it
#find_package(yaml-cpp REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wcast-qual \
#	-Wconversion -Wunreachable-code -Wmissing-noreturn -Wshadow \
#	-Wfatal-errors -pedantic")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

FILE(GLOB_RECURSE SRC src/*.cpp)

include_directories(${PROTO_SRC_DIR} include)

add_definitions(-DSPDLOG_NO_DATETIME -DSPDLOG_NO_THREAD_ID -DSPDLOG_NO_NAME)
	#-DSPDLOG_FMT_EXTERNAL) FIXME: Link errors

add_executable(yampd
   ${SRC}
   ${PROTO_SRCS}
   ${GRPC_SRCS}
)

target_link_libraries(yampd
	  boost_system boost_filesystem
    sfml-system sfml-audio
    gRPC::grpc++_reflection
    protobuf::libprotobuf
    yaml-cpp mpg123 tag
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