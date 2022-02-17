#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
char ans[1024];
void eval(char *tmp2){
	long long int val = -1, index1 = 0, index2 = 0, i = 0;
	char num1[1024], num2[1024], symbol = '!';
	while(i < strlen(tmp2)){
		while(i < strlen(tmp2) && (tmp2[i] != '+' && tmp2[i] != '-' && tmp2[i] != '*' && tmp2[i] != '/')){
			num1[index1++] = tmp2[i++];
		}
		symbol = tmp2[i++];
		while(i < strlen(tmp2)){
			num2[index2++] = tmp2[i++];
		}	
	}
	if(symbol == '+'){
		val = atoi(num1) + atoi(num2);
		sprintf(ans, "%lld", val);
	}
	else if(symbol == '-'){
		val = atoi(num1) - atoi(num2);
		sprintf(ans, "%lld", val);
	}
	else if(symbol == '*'){
		val = atoi(num1) * atoi(num2);
		sprintf(ans, "%lld", val);
	}
	else if(symbol == '/'){
		double x = (double)atoi(num1) / atoi(num2);
		sprintf(ans, "%lf", x);
	}
}
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
		char tmp1[1024], tmp2[1024];
		bzero(bf, 1024);
		recv(client_sock, bf, sizeof(bf), 0);
		
		sscanf(bf, "%s %*s", tmp1);
		printf("Connected with %s\n\n[%s]: ", tmp1, tmp1);
		sscanf(bf, "%*s %s", tmp2);
		printf("%s\n", tmp2);
		
		eval(tmp2);
		send(client_sock, ans, strlen(ans), 0);
		printf("[Server]: %s", ans);
		printf("\n");
	}
	close(client_sock);
	printf("Client left the chat and you ended the call!!\n");
	return 0;
	
	} 

