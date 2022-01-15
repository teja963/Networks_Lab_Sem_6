#include<stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/stat.h>
#define BUFF_MAX 500
#define SIZE 500
#define PORT_NUMBER 8021
#define SA struct sockaddr
#define MAX_LINE 500
#define BUFFSIZE 500

ssize_t total=0;

void sendfile(FILE *fp, int sockfd) 
{
    int n; 
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
	return;
}
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
// Driver function 
int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
	int n; 
	// socket create and verification ; IpV4 PROTOCOL; TCP; IP=0
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	memset(&servaddr,0, sizeof(servaddr));
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT_NUMBER);

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

    char *filename = "send";
	while(1)
	{
		strcpy(buff2, "gibberish\n");
		if(recv(connfd, &buff1, 500, 0) < 0)
			perror("Error in recv()");
		else if(strcmp(buff1, "Bye\n") == 0)
		{
			close(connfd);
			break;
		}
		else if(strcmp(buff1, "GivemeyourVideo\n") ==0)
		{
			printf("[+]Client : %s", filename);
			//exit(0);
			FILE *fp = fopen(filename, "r");
			struct stat st;
			char buffer[32];
			stat(filename, &st);
			int sz = st.st_size;
			itoa(sz, buffer);
	
			send(connfd, buffer, sizeof(buffer), 0);
			printf("[+]File opened.\n");
			if(fp == NULL)
			{
				perror("File Open");
				break;
			}
			sendfile(fp, connfd);
			char file_end[6];
			strcpy(buff2,"end");
			
			printf("[+]Successfully sent file.\n");
		}
		else
		{
			printf("[+]Client : %s", buff1);
			send(connfd, buff1, strlen(buff1)+1, 0);
			printf("hdfc");
		}
		memset(&buff1, 0, strlen(buff1)+1);
		memset(&buff2, 0, strlen(buff2)+1);
	}    
	close(sockfd);
} 