#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#define MAX_SIZE 1024
void black () {
  printf("\033[1;30m");     //  Escape is: \033
}                           // Color Code is: [0;30m

void red() {
  printf("\033[1;31m");
}

void green() {
  printf("\033[1;32m");
}

void yellow() {
  printf("\033[1;33m");
}

void blue() {
  printf("\033[1;34m");
}

void purple() {
  printf("\033[1;35m");
}

void cyan() {
  printf("\033[1;36m");
}

void reset () {
  printf("\033[0m");           //Resets the text to defaults
}
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5000;
	  int sock_server, sock_client;
	  struct sockaddr_in server_addr, client_addr;
	  socklen_t  addr_size;
	  int n;
	  char buffer[MAX_SIZE];
	  /*   socket connection protocol(domain, type, protocol)   */
	  sock_server = socket(AF_INET, SOCK_STREAM, 0);
	  if(sock_server < 0)
	  {
	     red();
	     printf("[-]Socket Connection is not established!!\n");
	     reset();
	     exit(1);
	  }
	  else
	  {
	     green();
	     printf("[+]Succesfully Established Socket connection!!\n");
	     reset();
	  }
	  /* Intializing server_addr by memset takes pointers thats here server_addr is struct type*/
	  memset(&server_addr, 0, sizeof(server_addr));
	  server_addr.sin_family = AF_INET;
	  server_addr.sin_port   = port;
	  server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	  /*  Binding protocol (sock_connection, struct sockaddr* server_addr, size)   */
	  n = bind(sock_server, (struct sockaddr*)&server_addr, sizeof(server_addr));
	  if(n > 0)
	  {
	     red();
	     printf("[-]Binding Connection is not established!!\n");
	     reset();
	     exit(1);
	  }
	  else
	  {
	     green();
	     printf("[+]Succesfully Established Binding connection!!\n");
	     reset();
	  }
	  return 0;
  }

