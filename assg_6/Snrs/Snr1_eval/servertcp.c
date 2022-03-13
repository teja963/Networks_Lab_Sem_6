#include<stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFF_MAX 500
#define SIZE 500
#define PORT_NUMBER 8041
#define SA struct sockaddr
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
void writefile(FILE *fp,int sockfd)
{
	char buff1[BUFF_MAX], buff2[BUFF_MAX], buffer[500]; 
    //FILE *fp = fopen("recv.txt", "w");
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
		
	}
	fclose(fp);
	printf("[+]The file has been recieved.\n");
	return;
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
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
	return;
}
void listdir(int sockfd)
{
	DIR *d;
	int count = 0;
	struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
			count++;
            
        closedir(d);
    }
	printf("%d \n", count);
	char buffer[32];
	itoa(count, buffer);
	send(sockfd, buffer, sizeof(buffer), 0);
	char buff1[BUFF_MAX], buff2[BUFF_MAX];
	d = opendir(".");
	if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            strcpy(buff2, dir->d_name);
			send(sockfd, buff2, sizeof(buff2), 0);

        }
        closedir(d);
	}

}

int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX], bu[32]; 
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
	char * line = NULL;
    size_t len2 = 0;
    ssize_t nbb;
	int flag = 0;
	
	FILE *ap = fopen("logincred.txt", "r");
	if(ap == NULL)
		exit(0);
	else
	{
		while(flag!=1){
			recv(connfd, &buff1, 500, 0);
			buff1[strcspn(buff1, "\n")] = 0;
			FILE *ap = fopen("logincred.txt", "r");
			while ((nbb = getline(&line, &len2, ap)) != -1) {
			if(strstr(line, buff1)!=NULL)
			{
				strcpy(buff2, "OK");
				send(connfd, buff2, strlen(buff2)+1, 0);
				recv(connfd, &buff1, 500, 0);
				buff1[strcspn(buff1, "\n")] = 0;
				if(strstr(line, buff1)!=NULL)
				{
					strcpy(buff2, "OK");
					send(connfd, buff2, strlen(buff2)+1, 0);
					// /printf("%s ", buff1);
					flag = 1;
					break;
				}
				else
				{
					bzero(buff2, sizeof(buff2));
					strcpy(buff2, "givversi");
					send(connfd, buff2, strlen(buff2)+1, 0);
				}
			}

   			}
		   if(flag == 0)
			{
				bzero(buff2, sizeof(buff2));
				strcpy(buff2, "nousername");
				send(connfd, buff2, strlen(buff2)+1, 0);
				FILE *ap = fopen("logincred.txt", "r");
				//exit(0);
			}
		}
	}
	


    char filename[32] = "s1.png";
	
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
		else if(strcmp(buff1, "GetFile\n") ==0)
		{
			bzero(filename, sizeof(filename));
			recv(connfd, &filename,32 , 0);
			printf("%s", filename);
			filename[strcspn(filename, "\n")] = 0;
			FILE *fp = fopen(filename, "r");
			struct stat st;
			char buffer[32];
			stat(filename, &st);
			int sz = st.st_size;
			itoa(sz, buffer);
			printf("%d\n", sz);
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
		else if(strcmp(buff1, "StoreFile\n") ==0)
		{
			memset(filename, 0, sizeof(filename));
			recv(connfd, &filename,32, 0);
			char filee[32] = "lala.png";
			printf("%s", filename);
			FILE *fp = fopen(filename, "w");
			writefile(fp, connfd);
		}
		else if(strcmp(buff1, "ListDir\n") ==0)
		{
			listdir(connfd);
		}
		else
		{
			printf("[+]Client : %s", buff1);
			send(connfd, buff1, strlen(buff1)+1, 0);
		}
		memset(&buff1, 0, strlen(buff1)+1);
		memset(&buff2, 0, strlen(buff2)+1);
	}    
	close(sockfd);
} 