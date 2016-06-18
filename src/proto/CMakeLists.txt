set (CMAKE_VERBOSE_MAKEFILE ON)
set (PROTOFILE master.proto worker.proto)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTOFILE})
message("pb sources: ${PROTO_SRCS}\npb headers: ${PROTO_HDRS}")

PROTOBUF_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${PROTOFILE})
message("grpc sources: ${GRPC_SRCS}\ngrpc headers: ${GRPC_HDRS}")

add_library(rpc ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
