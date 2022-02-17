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
	char bf[1024];
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
		printf("You: ");
		bzero(bf, 1024);
		fgets(bf, 1024, stdin);
		send(sock, bf, strlen(bf), 0);
		
		printf("Server: ");
		bzero(bf, 1024);
		recv(sock, bf, sizeof(bf), 0);
		printf("%s\n", bf);
		
		if(strcmp(bf, "Bye\n") == 0)break;
	}
	close(sock);
	printf("Server Ended the call!!!\n");
	
	return 0;
	}

