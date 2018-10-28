#one receive and response client-server
CC = clang++-6.0
all: client server multithread-server
client: client.cpp 
	$(CC)  $? -o $@
server: server.cpp
	$(CC)  $? -o $@ 
multithread-server: multithread-server.cpp
	$(CC) -std=c++11 $? -o $@ -pthread
clean:
	rm client
	rm server
	rm multithread-server
