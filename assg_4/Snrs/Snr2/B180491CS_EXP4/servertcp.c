#include<stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/select.h> 
#define MAXSIZE 80 
#define PORT_NUMBER 3000
#define SA struct sockaddr


// Driver function 
int main() 
{ 
	int sockfd, connfd, len; 
	int fdmax; // max file desc number.
	struct sockaddr_in servaddr, cli; 
	char ibuff[255],obuffer[255];
	char buff[MAXSIZE]; 
	char username[32], message[300];
	int n, nbytes ; 
	char buf[255];
	char names[300][32];
	int yes = 1;
	// master file descriptor set
	fd_set master;
	// temp file descriptor
	fd_set read_fds;

	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	memset(names, 0,  (sizeof *names) * 32);
 
	// socket create and verification ; IpV4 PROTOCOL; TCP; IP=0
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	memset(&servaddr,0, sizeof(servaddr));
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT_NUMBER); 
	
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("Server-setsockopt() error lol!");
		exit(1);
	}
	printf("Server-setsockopt() is OK...\n");
	

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Socket bind failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded\n"); 
	FD_SET(sockfd, &master);
	// Now server is ready to listen and verification 
	if ((listen(sockfd, 50)) != 0) { 
		printf("Listen failed\n"); 
		exit(0); 
	} 
	else
		printf("Server listening\n"); 
	//add sockfd to the master set
	
	// so far this is the biggest file descriptor
	fdmax = sockfd;
	//printf("%d\n", sockfd);
	//Loop now
	bzero(names, sizeof(names));
	while(1)
	{
		read_fds = master;
		// read write and null
		if(select(FD_SETSIZE+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("Server-select() error lol!");
			exit(1);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{		
			if(FD_ISSET(i, &read_fds))
			{
				if(i == sockfd)
				{
					// handle new connections
					//printf("here");
					len = sizeof(cli);
					if((connfd = accept(sockfd, (struct sockaddr *)&cli, &len)) == -1)
					{
						perror("Server-accept() error lol!");
					}
					else
					{
						FD_SET(connfd, &master); /* add to master set */
						if(connfd > fdmax)
						{
							fdmax = connfd;
						}
						// receive the username of the client
						memset(username,0,sizeof(username));
						memset(names[connfd],0,sizeof(names[connfd]));
	                    read(connfd,names[connfd],sizeof(names[connfd]));
						strcpy(username, names[connfd]);
						memset(message, 0, sizeof(message));
						strcpy(message, username);
						strcat(message, " has joined the chat!");
						printf("New connection from (username: %s) on socket %d\n", username, connfd);
						char namestr[255];
						int flag = 1;
						bzero(namestr, sizeof(namestr));
						strcpy(namestr, "Active users are: \n");
						for (int k = 0; k <= fdmax; k++)
						{
							if (FD_ISSET(k, &master) &&  (k != sockfd && k != connfd))
							{
								
								flag = 0;
								strcat(namestr, names[k]);
								strcat(namestr, "\n");
							}
						}
						if(flag == 1)
							strcat(namestr, "Nil");
						strcat(namestr, "\n");
						
						write(connfd, namestr, sizeof(namestr));
						for (int j = 0; j <= FD_SETSIZE; j++)
						{
							if(FD_ISSET(j, &master)){
								if(j != sockfd && connfd != j)
								{		
									if(write(j, message,sizeof(message) ) == -1)
										perror("send() error lol!");
									if(strncmp("tata",ibuff,4)==0)
									{
										printf("Connection Terminated by You\n");
										break;
									}

								}
							}
							
						}
					}				
				}
				// handle the data from the client
				else
				{
					bzero(ibuff, 255);
					if((nbytes = recv(i, ibuff, sizeof(ibuff), 0)) <= 0)
					{
						if(nbytes == 0)
							printf("%s: socket %d hung up\n", username, i);
						else
							perror("recv error");
						close(i);
						// Remove from the master set
						FD_CLR(i, &master);
					}
					else
					{
						// we got data from the client;
						// send data to everyone
						
						memset(message, 0, sizeof(message));
						strcpy(message, names[i]);
						strcat(message, ": ");
						strcat(message, ibuff);
						//char newline [] = "/n";
						//message[strcspn(message,newline)] = '\0';
						//printf("%s\n",message);
						if(strncmp("tata",ibuff,4)==0)
						{
							write(i, message,sizeof(message));
							printf("Connection Terminated by the client %s\n", names[i]);
							//break;
						}
						else
						printf("%s: %s\n", names[i], ibuff);
						for (int j = 0; j <= FD_SETSIZE; j++)
						{
							if(FD_ISSET(j, &master)){
								if(j != sockfd && j != i)
								{		
									if(write(j, message,sizeof(message) ) == -1)
										perror("send() error lol!");
									
								}
							}
						}
						if(strncmp("tata",ibuff,4)==0)
						{
							FD_CLR(i, &master);
						}
					}
					memset(ibuff, 0, sizeof(ibuff));
					memset(message, 0, sizeof(message));
				}
			}
		}
	}
	close(sockfd); 
} 
