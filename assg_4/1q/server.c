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
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_size;
	char bf[1024];
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0){
		perror("[-]Socket Connection is not established\n");
		exit(1);
	}
	else{
		printf("[+]Socket Established\n");
	}
	
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	int n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0){
		perror("[-]Failed to binded\n");
		exit(1);
	}
	else{
		printf("[+]Successfully Binded together\n");
	}
	
	listen(server_sock, 5);
	printf("[+]Listening.........\n");
	
	addr_size = sizeof(client_addr);
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
	printf("[+]Client Connected!!\n");
	
	while(1){
		printf("Client: ");
		bzero(bf, 1024);
		recv(client_sock, bf, sizeof(bf), 0);
		printf("%s", bf);
		
		printf("You: ");
		bzero(bf, 1024);
		fgets(bf, 1024, stdin);
		send(client_sock, bf, strlen(bf), 0);
		
		if(strcmp(bf, "Bye\n") == 0)break;
		printf("\n");
	}
	close(client_sock);
	printf("Client left the chat and you ended the call!!\n");
	return 0;
	
	} 

