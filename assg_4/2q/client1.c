#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
	char *ip = "127.0.0.1";
	int port = 7575;
	int sock;
	struct sockaddr_in addr;
	socklen_t addr_size;
	char bf1[1024], bf2[1024];
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("[-]Socket Connection is not established\n");
		exit(1);
	}
	else{
		printf("[+]Socket Established\n");
	}
	
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = inet_addr(ip);
	
	connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	printf("[+]Connected to server!!!\n");
	
	while(1){
		printf("enter the query: ");
		bzero(bf1, 1024);
		bzero(bf2, 1024);
		fgets(bf2, 1024, stdin);
		sprintf(bf1, "Client1 ");
		strcat(bf1, bf2);
		send(sock, bf1, strlen(bf1), 0);
		
		if(strcmp(bf2, "Exit\n") == 0)break;
		printf("[Server]: ");
		bzero(bf1, 1024);
		recv(sock, bf1, sizeof(bf1), 0);
		printf("%s\n", bf1);
		
	}
	close(sock);
	printf("[-]You Disconnected the call!!!\n");
	
	return 0;
	}

