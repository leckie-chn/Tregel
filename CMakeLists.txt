cmake_minimum_required(VERSION 2.8)
project(Tregel)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

find_package(GRPC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(LevelDB REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(src/master)
add_subdirectory(src/worker)
add_subdirectory(src/proto)
add_subdirectory(src/util)
