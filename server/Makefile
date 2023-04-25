CC=g++

EXTRAFLAGS=-lpqxx -lpq -lpthread
GOOGLE_BUFFER= `pkg-config --cflags --libs protobuf`
PROTO= protobuf/amazon_ups.pb.cc protobuf/world_amazon.pb.cc 

server: server.cpp
	$(CC) -std=c++11 -g -o $@ server.cpp $(PROTO) $(GOOGLE_BUFFER) $(EXTRAFLAGS)


.PHONY:
	clean
clean:
	rm -rf *.o *.cpp~ *.hpp~ *# main
