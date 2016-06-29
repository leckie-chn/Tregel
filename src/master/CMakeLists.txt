include_directories(${PROJECT_BINARY_DIR}/src/proto)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable(master master.cc masterImpl.cc)
target_link_libraries(master rpc util ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})
