#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#define BUFF_MAX 500
#define SIZE 500
#define PORT 8041
#define SA struct sockaddr_in
#define MAX_LINE 500
#define BUFFSIZE 500
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
void sendfile(FILE *fp, int sockfd) 
{
    int n; 
	ssize_t total=0;
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        //printf("%s", sendline);
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
	return;
}

void writefile(int sockfd, FILE *fp)
{
	char buff1[BUFF_MAX], buff2[BUFF_MAX], buffer[500]; 
    
	int k  = 0;
	ssize_t total=0;
	int nb = read(sockfd, &buffer, 500);
	//printf("%d\n", nb);
	//printf("%s \n", buffer);
	int x = atoi(buffer);
	nb = 0;
	while (total <= x) 
	{
		memset(buff2, 0, MAX_LINE);
		nb = read(sockfd, buff2, MAX_LINE);
		total+=nb;
		//printf("%s", buff2);
		if (fwrite(buff2, sizeof(char), nb, fp) != nb)
		{
			perror("Write File Error");
			exit(1);
		}
		if(total >= x)
			break;
		//printf("%d ", total);
	}
	fclose(fp);
	printf("[+]The file has been recieved.\n");
	return;
}

void listdir(int sockfd)
{
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
	int nb = read(sockfd, &buff2, 500);
	printf("%d\n", nb);
	printf("%s \n", buff2);
	int x = atoi(buff2);
	for(int i = 0; i < x; i++)
	{
		nb = read(sockfd, buff2, MAX_LINE);
		printf("%s\n", buff2);
	}
	printf("[+]The things are done.\n");
}
int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX], bu[32]; 
	int n, len; 
	fgets(bu, 32, stdin);
  	if(strcmp(bu,"START\n") == 0)
		len = 1;
	else
		exit(0);

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
		printf("200 OK - Connected to the server.\n"); 
	bzero(buff1, sizeof(buff1));
	bzero(buff2, sizeof(buff2));
	int nb;
    char filename[32] = "s2.jpg";

	while(1)
	{
		fgets(bu, 32, stdin);
		if(strcmp(bu,"USERN\n") == 0)
		{
			bzero(buff1, sizeof(buff1));
			bzero(buff2, sizeof(buff2));
			fgets(buff1, 500, stdin);
			send(sockfd, buff1, strlen(buff1), 0);
			recv(sockfd, &buff2, 500, 0);
			if(strcmp(buff2, "OK") == 0)
			{
				printf("300 - Need password now\n");
				fgets(bu, 32, stdin);
				if(strcmp(bu,"PASSWD\n") == 0)
				{
					fgets(buff2, 500, stdin);
					send(sockfd, buff2, strlen(buff1), 0);
					recv(sockfd, &buff2, 500, 0);
					if(strcmp(buff2, "OK") == 0)
					{
						printf("305 - Donee\n");
						break;
					}
					else
					{
						printf("310 - Wrong password; start from USERN again\n");
						continue;
					}
				}
			}
			else
			{
				printf("301 - Wrong user nam; start from USERN again\n");
				continue;
			}
		}
		else
		{
			printf("Need username, enter USERN and start again");	
			continue;
		}
	}

	
	printf("Welcome! %s\n", buff1);

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
            if(strcmp(buff1,"GetFile\n") == 0)
			{
				printf("Enter the file name: ");
				fgets(bu, 32, stdin);	
				bu[strcspn(bu, "\n")] = 0;
				FILE *fp = fopen(bu, "w");			
				send(sockfd, bu, strlen(bu), 0);
				writefile(sockfd, fp);

			}
			else if (strcmp(buff1 , "ListDir\n") == 0) 
			{
				listdir(sockfd);
			}
			else if (strcmp(buff1 , "StoreFile\n") == 0) 
			{
				
				memset(filename, 0, sizeof(filename));
				fgets(filename, 32, stdin);	
			//	printf("%s\n", filename);			
				send(sockfd, filename, strlen(filename)+1, 0);
				filename[strcspn(filename, "\n")] = 0;
				FILE *fp = fopen(filename, "r");
				struct stat st;
				char buffer[500];
				stat(filename, &st);
				int sz = st.st_size;
				itoa(sz, buffer);
				send(sockfd, buffer, sizeof(buffer), 0);
				//printf("%s", buffer);
				printf("[+]File opened.\n");
				if(fp == NULL)
				{
					perror("File Open");
					break;
				}
				sendfile(fp, sockfd);

			}
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