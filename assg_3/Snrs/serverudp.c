// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 8080 
#define MAXLINE 1024 

// Driver code 
int main() { 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *hello = "Hello from server"; 
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	}
    else{
        printf("successfully created the socket!!\n");
    }
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
    else{
        printf("Bind successful!!\n");
		printf("Started listening...\n");
    }
	
	int len, n; 

	len = sizeof(cliaddr); //len is value/resuslt 
    while(1)
    {
        memset(buffer,0,MAXLINE); 
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
        printf("Message from the Client : %s", buffer); 
        memset(buffer,0,MAXLINE); 
        n = 0; 
        printf("Message to the Client: ");
		// copy server message in the buffer 
		while ((buffer[n++] = getchar()) != '\n') 
			; 
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
     //   printf("Hello message sent.\n"); 
        if ((strncmp(buffer, "exit", 4)) == 0) { 
			printf("Server Exit\n"); 
			break; 
		} 
    }

	
	
	close(sockfd);
	
	
	return 0; 
} 
