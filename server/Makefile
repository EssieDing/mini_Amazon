CC=g++

EXTRAFLAGS=-lpqxx -lpq -lpthread
GOOGLE_BUFFER= `pkg-config --cflags --libs protobuf`
PROTO= protobuf/amazon_ups.pb.cc protobuf/world_amazon.pb.cc 
CXXFLAGS = -std=c++17 -O2 -g -mcmodel=large
SERVER= common.cpp server.cpp
AUX = tinyxml2.cpp
TASKFLOW = -I.

server: server.cpp common.cpp
	$(CC) $(CXXFLAGS) $(TASKFLOW) -o $@ $(SERVER) $(PROTO) $(AUX) $(GOOGLE_BUFFER) $(EXTRAFLAGS)


.PHONY:
	clean
clean:
	rm -rf *.o *.cpp~ *.hpp~ *# main
