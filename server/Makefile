CC=g++

EXTRAFLAGS=-lpqxx -lpq -lpthread
GOOGLE_BUFFER= -pkg-config --cflags --libs protobuf

server: server.cpp
	$(CC) -std=c++11 -g -o $@ server.cpp $(GOOGLE_BUFFER) $(EXTRAFLAGS)


.PHONY:
	clean
clean:
	rm -rf *.o *.cpp~ *.hpp~ *# main
