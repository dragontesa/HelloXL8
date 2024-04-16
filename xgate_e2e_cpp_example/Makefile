CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CPPFLAGS += -I. -I/usr/local/include
CXXFLAGS += -std=c++14
LDFLAGS += -L/usr/local/lib `pkg-config --libs --static protobuf grpc++` \
           -pthread\
           -lgrpc++_reflection\
           -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./protos

vpath %.proto $(PROTOS_PATH)

all: wave_to_text

wave_to_text: google/api/http.pb.o google/api/annotations.pb.o e2e_pipe/api/e2e_api_layer_proto.pb.o e2e_pipe/api/e2e_api_layer_proto.grpc.pb.o wave_to_text.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -rf google e2e_pipe wave_to_text *.o