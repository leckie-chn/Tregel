include_directories(${PROJECT_BINARY_DIR}/src/proto)

add_executable(worker worker.cc workerImpl.cc)
target_link_libraries(worker rpc ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})