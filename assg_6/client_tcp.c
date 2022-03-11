#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5001;
	  int sock;
	  struct sockaddr_in addr;
	  socklen_t  addr_size;
	  int n;
	  char bf1[1024], bf2[1024];
	  /*   socket connection protocol(domain, type, protocol)   */
	  sock = socket(AF_INET, SOCK_STREAM, 0);
	  if(sock < 0)
	  {
	                                                                                                                 
	     perror("[-]Socket Connection is not established!!\n");
	                                                                                                                 
	     exit(1);
	  }
	  else
	  {
	                                                                                                                
	     printf("[+]Succesfully Established Socket connection!!\n");
	                                                                                                                
	  }
	  /* Intializing addr by memset takes pointers thats here addr is struct type*/
	  memset(&addr, '\0' , sizeof(addr));
	  addr.sin_family = AF_INET;
	  addr.sin_port   = port;
	  addr.sin_addr.s_addr = inet_addr(ip_addr);
	  
	  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	  printf("[+]Connected to server\n\n");
	  
	  /*Client request server starts*/
	 
	  return 0;
	  }
