CC=g++
CFLAGS=-Wno-unused-variable

all: server client

server: src/server.cpp src/zeromq.o
	$(CC) $(CFLAGS) -o server src/server.cpp src/zeromq.o -lzmq

client: src/client.cpp src/zeromq.o
	$(CC) $(CFLAGS) -o client src/client.cpp src/zeromq.o -lzmq

src/zeromq.o: src/zeromq.cpp src/zeromq.hpp
	$(CC) $(CFLAGS) -c -o src/zeromq.o src/zeromq.cpp -lzmq

clean:
	rm -f server client src/zeromq.o
