#include<stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#define BUFF_MAX 80 
#define PORT_NUMBER 8080 
#define SA struct sockaddr


// Driver function 
int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in6 servaddr, cli; 
	char buff[BUFF_MAX]; 
	int n; 
	// socket create and verification ; IpV4 PROTOCOL; TCP; IP=0
	sockfd = socket(AF_INET6, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	memset(&servaddr,0, sizeof(servaddr));
	// assign IP, PORT 
	servaddr.sin6_family = AF_INET6; 
	servaddr.sin6_addr = in6addr_any; 
	servaddr.sin6_port = htons(PORT_NUMBER); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Socket bind failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed\n"); 
		exit(0); 
	} 
	else
		printf("Server listening\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("Server acccept failed\n"); 
		exit(0); 
	} 
	else
		printf("Server acccept the client\n"); 

	// Function for chatting between client and server 
	//
	
	// Continuously keep listening  
	while(1){
        // clear the buffer
		memset(buff,0,BUFF_MAX); 
		// read the message from client and copy it in buffer 
		read(connfd, buff, sizeof(buff)); 
		// print buffer which contains the client contents 
		printf("From client: %sTo client : ", buff); 

		memset(buff,0,BUFF_MAX); 
		n = 0; 
		// copy server message in the buffer 
		while ((buff[n++] = getchar()) != '\n') 
			; 

		// and send that buffer to client 
		write(connfd, buff, sizeof(buff)); 

		// if msg contains "Exit" then server exit and chat ended. 
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} 
	}
	// After chatting close the socket 
	close(sockfd); 
} 
