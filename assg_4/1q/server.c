#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>

static _Atomic unsigned int client_count  = 0;   //Need to refer
static int uid  = 10;

#define MAX_CLIENT 100
int main(){
	char *ip = "127.0.0.1";
	int port = 7575, option = 1;
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_size;
	char bf[2048];
	pthread_t tid;           //We are using threading for chat
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
	
	signal(SIGPIPE, SIG_IGN);   //signals software generatic interupts 
	
	if(setsockopt(server_sock, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option)) < 0){ //Checking for proper connection
		printf("[-]There is an error on setsockopt\n");
		exit(1);
	}
	
	int n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0){
		perror("[-]Failed to binded\n");
		exit(1);
	}
	else{
		printf("[+]Successfully Binded together\n");
	}
	
	if(listen(server_sock, 10) < 0){
		printf("[-]Error in listening\n");
	}
	else{
		printf("\t****WELCOME TO CHAT ROOM****\n");
	}
	
	
	while(1){
		addr_size = sizeof(client_addr);
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
		
		//check for MAX_CLIENTS
		if(client_count + 1 == MAX_CLIENT){
			printf("Maximum Client connected!!Unable to connect new clients\n");
			close(client_sock);
		}
		
		
		
		
		
		printf("Client: ");
		bzero(bf, 2048);
		recv(client_sock, bf, sizeof(bf), 0);
		printf("%s", bf);
		
		printf("You: ");
		bzero(bf, 2048);
		fgets(bf, 2048, stdin);
		send(client_sock, bf, strlen(bf), 0);
		
		if(strcmp(bf, "Bye\n") == 0)break;
		printf("\n");
	}
	close(client_sock);
	printf("Client left the chat and you ended the call!!\n");
	return 0;
	
	} 

