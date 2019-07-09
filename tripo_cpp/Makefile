LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -Wl,-rpath,/usr/local/lib -lrestclient-cpp\
           -L/usr/local/boost_1_56_0/lib -lboost_thread -lboost_system\
           -Wl,-rpath,/usr/local/boost_1_56_0/lib

SOURCES = Health.pb.cc Health.grpc.pb.cc 
SOURCES += $(wildcard *.cc)
OBJS = $(patsubst %.cc, %.o, $(SOURCES))

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -I../include -I/usr/local/boost_1_56_0/include -Wall -std=c++11

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

TRIPO_SO = libtripo_cpp.so
TRIPO_LIB = libtripo_cpp.a

all: $(TRIPO_LIB) $(TRIPO_SO)

$(TRIPO_SO): $(OBJS)
	echo $^
	g++ -shared -o $@ $(OBJS)

$(TRIPO_LIB): $(OBJS)
	echo $^
	ar cr $@ $^
	ranlib $@

%.o : %.cc
	$(CXX) -fPIC $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

%.grpc.pb.cc: %.proto
	protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc : %.proto
	protoc -I. --cpp_out=. $<

clean:
	rm -frv $(TRIPO_SO) $(TRIPO_LIB) *.o *.pb.cc *.pb.h

