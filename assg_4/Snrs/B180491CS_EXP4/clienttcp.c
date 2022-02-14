#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include<sys/select.h>
#include <arpa/inet.h>
#define MAXSIZE 255
#define PORT 3000
#include <fcntl.h>
#define SA struct sockaddr_in

int main(int argc, char *argv[])
{ 
	if(argc<2)
    {
        printf("Enter the port number to continue...\n");
        exit(1);
    }
	struct timeval timeout;  
    timeout.tv_sec = 1;
    timeout.tv_usec = 100;
	fd_set sets;
    FD_ZERO(&sets);
	int sockfd, connfd, k =0; 
	char name[32] = " ";
	struct sockaddr_in servaddr, cli; 
	char buff[MAXSIZE], buff2[MAXSIZE]; 
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
	// assign IP, PORT 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);//htons(atoi(argv[1]));//Use the port number given 
	printf("Enter your name: ");
	n = 0; 
	while ((name[n++] = getchar()) != '\n') ; 
	int size = strlen(name); //Total size of string
	name[size-1] = '\0';
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server.\n"); 
	
	printf("----------------WELCOME TO THE CHAT ROOM------------\n");
	printf("welcome to the chat! %s\n", name);
	
	n = write(sockfd,name,strlen(name));//Write the accepted message into the socket
    if(n<0)//Write failed
    {
        perror("error writing client");
        exit(1);
    }
	int flags=fcntl(0, F_GETFL);
    fcntl(0, F_SETFL,flags| O_NONBLOCK);
	bzero(buff, sizeof(buff));
    bzero(buff2, sizeof(buff2));

	while(1)
    {
        bzero(buff, sizeof(buff));
   		bzero(buff2, sizeof(buff2));
		scanf("%[^\n]s", buff);
        buff[strcspn(buff,"\n")] = 0;
        if(write(sockfd,buff,strlen(buff))<0)//Write the accepted message into the socke
        {
            perror("Cant write from the client");
            exit(1);
        }
        if(strncmp("tata",buff,4)==0)
        {
            printf("You left the chat\n");
            break;
        }
        memset(buff,0, sizeof(buff));
   		memset(buff2,0, sizeof(buff2));
        FD_SET(sockfd, &sets);
        n= select(sockfd+1,&sets,NULL,NULL,&timeout);//Read message from the socket
		if(n==0)
        	continue;
        else
        {
             n=read(sockfd,buff2,sizeof(buff2));
			 if(n == 0)
			 {
				 printf("Server hung up..\n");
				 break;
			 }
			 len = strlen(buff2);
			 if( buff2[len-1] == '\n' )
    				buff2[len-1] = 0;
		}
        if(n<0)
        {
            perror("Error reading other messages");
            exit(1);
        }
		if( strstr(buff2, "tata") != NULL)
		{
			char strnew[33];
			char* a = strchr(buff2, ':');
			int b = (a == NULL ? -1 : a - buff2);
			printf("%.*s", b, buff2);
			printf(" has left the chat :/ \n");
			memset(buff,0, sizeof(buff));
   			memset(buff2,0, sizeof(buff2));
			continue;
		}
		if(!(strcmp(buff2, " ")&&buff2[0]=='\0')) 
        printf("%s\n",buff2);

		memset(buff,0, sizeof(buff));
   		memset(buff2,0, sizeof(buff2));
        
    }
    close(sockfd); //Close the socket
}