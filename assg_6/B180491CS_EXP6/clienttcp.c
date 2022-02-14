#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define BUFF_MAX 500
#define SIZE 500
#define PORT 8024
#define SA struct sockaddr_in
#define MAX_LINE 500
#define BUFFSIZE 500


void writefile(int sockfd)
{
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
    FILE *fp = fopen("recv", "w");
	int k  = 0;
	ssize_t total=0;
	int nb = read(sockfd, &buff2, 500);
	printf("%d\n", nb);
	printf("%s \n", buff2);
	long long int x = atoi(buff2);
	while (total <= x) 
	{
		total+=nb;
		if (nb < 2)
			break;
		
		if (fwrite(buff2, sizeof(char), nb, fp) != nb)
		{
			perror("Write File Error");
			exit(1);
		}
		if(total >= x)
			break;
		//printf("%d %d %d\n", total, nb,strstr(buff2,"end"));
		memset(buff2, 0, MAX_LINE);
		nb = read(sockfd, buff2, MAX_LINE);
	}
	printf("[+]The file has been recieved.\n");
	return;
}


int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
	int n, len; 
	// socket create and varification 
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
    servaddr.sin_port = htons(PORT);
	
	/* Connect to server the server */
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server.\n"); 
	bzero(buff1, sizeof(buff1));
	bzero(buff2, sizeof(buff2));
	int nb;
    char *filename = "read";
	while(1)
    {
        memset(buff1, 0, sizeof(buff1));
        memset(buff2, 0, sizeof(buff2));
		printf(">> ");
        fgets(buff1, 500, stdin);
		nb =0;
        if (strcmp(buff1 , "Bye\n") == 0) 
        {
            send(sockfd, buff1, strlen(buff1), 0);
            printf("You have closed connection with the Server.\n");
            exit(0);
        }
        else
        {	
            send(sockfd, buff1, sizeof(buff1), 0);
            if(strcmp(buff1,"GivemeyourVideo\n") == 0)
                writefile(sockfd);
            else
            {
                read(sockfd, &buff2, 500);
				printf("%s", buff2);
                memset(&buff2, 0, sizeof(buff2));
            }
            memset(&buff1, 0, sizeof(buff1));
			
        }
	}
	close(sockfd);
} 
