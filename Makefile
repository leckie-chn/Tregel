CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++` -lprotobuf -lpthread -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH = $(shell which $(GRPC_CPP_PLUGIN))

PROTO_PATH = ./src/proto
RPC_PATH = ./src/rpc
BIN_PATH = ./bin

vpath %.proto $(PROTO_PATH)
vpath %.cc ${RPC_PATH} ./src/master ./src/worker

all: master worker

master:	master.pb.o master.grpc.pb.o master.o
	$(CXX) $^ $(LDFLAGS) -o $(BIN_PATH)/$@

worker: master.pb.o master.grpc.pb.o worker.o
	${CXX} $^ ${LDFLAGS} -o $(BIN_PATH)/$@

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTO_PATH) --grpc_out=$(RPC_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTO_PATH) --cpp_out=$(RPC_PATH) $<

.PHONY: clean
	rm *.o 

