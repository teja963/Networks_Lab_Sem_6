#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#define BUFF_MAX 80 
#define PORT 8080 
#define SA struct sockaddr_in6

int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in6 servaddr, cli; 
	char buff[BUFF_MAX]; 
	int n; 
	// socket create and varification 
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
	//servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	inet_pton(AF_INET6, "::1", &servaddr.sin6_addr);
    servaddr.sin6_port = htons(PORT); 

	/* Connect to server the server */
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server.\n"); 
	//Keep taking the input
	while (1){ 
		memset(buff,0,sizeof(buff)); 
		printf("Message to the server : "); 
		n = 0; 
		while ((buff[n++] = getchar()) != '\n') ; 
		write(sockfd, buff, sizeof(buff));
		memset(buff,0,sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("Message from the Server : %s", buff); 
		if ((strncmp(buff, "exit", 4)) == 0) { 
			printf("Client Exit\n"); 
			break; 
		} 
	} 
	// close the socket 
	close(sockfd); 
} 
