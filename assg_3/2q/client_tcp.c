#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
/*
void write_file(int sock){
	FILE *fp;
	char *filename = "recv";
	char bf[500];
	
	fp = fopen(filename, "w");
	if(fp == NULL){
		perror("[+]Error in Wrting the file!!!\n");
	}
	int i = 0, n;
	bzero(bf, 500);
	
	while(1){
	        bzero(bf, 500);
	        n = recv(sock, bf, sizeof(bf), 0);
	        if(n < 0){
	        	break;
	        }
		fprintf(fp, "%s", bf);
		
		
	}

	
}
*/
int main(){
	char *ip = "127.0.0.1";
	int port = 5001;
	int sock;
	struct sockaddr_in addr;
	socklen_t addr_size;
	char bf1[1024], bf2[1024];
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("[-]Socket connection is not established\n");
		exit(1);
	}
	else{
		printf("[+]Successfully established socket connection\n");
	}
	
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = port;
	addr.sin_addr.s_addr = inet_addr(ip);
	
	connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	printf("[+]Successfully connected to server!!\n");
	
	
	while(1){
		
		bzero(bf1, 1024);
		fgets(bf1, 1024, stdin);
		send(sock, bf1, strlen(bf1)-1, 0);
	
		if(strcmp(bf1, "Bye\n") == 0){
		        printf("[+]Server Closed the connection\n");
			close(sock);
			break;
		}
		else if(strcmp(bf1, "Givemeyourvideo\n") == 0){
			
		 	char msg[500];
			bzero(msg, 500);
			if(recv(sock, msg, sizeof(msg), 0) > 0){
				if(strcmp(msg, "Sending video") == 0){
					FILE * fp;
					fp = fopen("recv", "w");
					int bytes;
					int i = 0;
					
					bzero(msg, 500);
					while(recv(sock, msg, sizeof(msg), 0) >= 1){
						
						if(strcmp(msg, "Finished") == 0)break;
						
						//acknowledgement
						char bf[500];
						sprintf(bf,"Received");
						
						send(sock, bf, strlen(bf), 0);
						
						fprintf(fp, "%s", msg);
						i += 500;
						if(i % 5000000 == 0)printf("%d mb received\n", i / 1000000);
						bzero(msg, 500);	
					}
					printf("[+]File Transfered finished\n");
						
				}
				else{
				printf("%s", msg);
				fflush(stdout);
				}
			}
			
		
		}
		
		else{
			bzero(bf1, 1024);
			recv(sock, bf1, sizeof(bf1), 0);
			printf("%s\n", bf1);
		}
		
	}
	
	
	return 0;
}
