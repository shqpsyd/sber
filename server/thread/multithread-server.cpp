// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <thread>
#include <iostream>
//unknown reason for sudden exit
class Server {
	inline bool eof(char* arr, int length) {
		printf("juding eof");
		int index = length - 3;
		for(int i = 0; i < 3; i++) 			
			if(end[i] != arr[i + index])
				return false;		
		return true;
	}
	public: 
			Server(int PORT = 8080);
			void handle(int);
			

	private: 
		char end[3];
	
};

void Server::handle(int socket) {	
	char buffer[1024] = {0};	
	do {
		int valread = read( socket , buffer, 1024); 
		printf("%s\n",buffer ); 
		char *message = buffer;
		send(socket , message , strlen(message) , 0);		
		printf("echo message sent\n");	
	} while(!eof(buffer, strlen(buffer)));
	printf("here");
	return ; 
}

Server::Server(int PORT){
	end[0] = 'e'; end[1] = 'o'; end[2] = 'f';
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address);
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// setsockt options 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	//let 8080 listen and put connections to listen queue
	printf("finish binding to port%d\n", PORT);
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	printf("listening\n");
	int count = 0;
	int prev = 0;

	
	while(true) {
		//deque a connection from listen queue and assign a new new socket to it
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}	
		if(new_socket != prev) {
			prev = new_socket;			
			std::thread t(std::bind(&Server::handle,this,new_socket));
			t.join();	
			count++;		
		}
		if(count == 2) {
			break;
		}
		
	}
	printf("server close");
}
int main(int argc, char const *argv[]) 
{
	Server server;
	return 0;

} 

