LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -Wl,-rpath,/usr/local/lib -lrestclient-cpp\
           -L/usr/local/boost_1_56_0/lib -lboost_thread -lboost_system\
           -Wl,-rpath,/usr/local/boost_1_56_0/lib

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -I../include -I/usr/local/boost_1_56_0/include -Wall -std=c++11
TRIPO_LIB = ../tripo_cpp/libtripo_cpp.a

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: mathtest_client mathtest_server

mathtest_client: mathtest.pb.o mathtest.grpc.pb.o mathtest_client.o
	$(CXX) $^ $(TRIPO_LIB) $(LDFLAGS) -o $@ 

mathtest_server: mathtest.pb.o mathtest.grpc.pb.o mathtest_server.o
	$(CXX) $^ $(TRIPO_LIB) $(LDFLAGS) -o $@

%.pb.o : %.pb.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h mathtest_client mathtest_server

