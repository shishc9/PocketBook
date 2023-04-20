cmake_minimum_required(VERSION 3.22.0)
project(pocket_server)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-std=c++14)
add_definitions(-Wall -g)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

message("------- include all source dir -------")
include_directories(
  ./
  ./LogServer
)
message("------- find all source files -------")
aux_source_directory(. SRC_DIR)
aux_source_directory(./LogServer SRC_DIR)

message("------- create exec bin -------")
add_executable(${PROJECT_NAME} ${SRC_DIR})