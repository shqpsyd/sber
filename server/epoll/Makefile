#one receive and response client-server
CC = clang++-6.0
all: client server multithread-server
client: client.cpp 
	$(CC)  $? -o $@
server: server.cpp
	$(CC)  $? -o $@ 
clean:
	rm client
	rm server
