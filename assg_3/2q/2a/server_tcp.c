#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>


//generate random string
void random_str(char *string)
{
	static const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz!@#$%^&*~?><";

	int n = sizeof(str) - 1;

	for(int i=0;i<10;i++)
		string[i] = str[rand() % n];
}

int main(){
	char *ip = "127.0.0.1";
	char *filename = "send";
	int port = 5001;
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_size;
	char bf1[1024], bf2[1024];
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0){
		perror("[-]Socket connection is not established\n");
		exit(1);
	}
	else{
		printf("[+]Successfully established socket connection\n");
	}
	
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	int n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0){
		perror("[+]Binding Failed!!!!\n");
		exit(1);
	}
	else{
		printf("[+]Server Successfully binded!!\n");
	}
	
	listen(server_sock, 1000);
	printf("[+]Listening................\n");
	
	addr_size = sizeof(client_addr);
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
	printf("[+]Client Connected\n");
	
	while(1){
		bzero(bf1, 1024);
		recv(client_sock, bf1, sizeof(bf1), 0);
		printf("%s\n", bf1);
		
		if(strcmp(bf1, "Enter") == 0 || strcmp(bf1, "enter") == 0){
			bzero(bf1, 1024);
			strcpy(bf1, "Now server will responds to your words!!");
			send(client_sock, bf1, strlen(bf1) ,0);	
		}
		else if(strcmp(bf1, "Givemeyourvideo") == 0){
			char msg[500], bf[500];
			sprintf(msg, "Sending video");
			send(client_sock, msg, strlen(msg), 0);
			
			FILE *fp = fopen(filename, "r");
			
			int bytes, i = 0;
			printf("File Transfer starting\n");
			while( (bytes = fread(msg, 1, 500, fp)) >= 1){
				send(client_sock, msg, strlen(msg), 0);
				
				//Conformation
				bzero(bf, 500);
				recv(client_sock, bf, sizeof(bf), 0);
				i += 500;
				if(i % 5000000 == 0)
					printf("%d mb sent\n", i/1000000);
			}
			sprintf(msg, "Finished");
			send(client_sock, msg, strlen(msg), 0);
			printf("%s\n", msg);
		}
		else{
			bzero(bf1, 1024);
			random_str(bf1);
			send(client_sock, bf1, strlen(bf1), 0);
		}
		
	}
	
	
	
	return 0;	
}
