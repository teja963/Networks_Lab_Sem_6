#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
char tmp[1024];
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5001;
	  int server_sock, client_sock;
	  struct sockaddr_in server_addr, client_addr;
	  socklen_t  addr_size;
	  int n;
	  char bf1[1024], bf2[1024];
	  /*   socket connection protocol(domain, type, protocol)   */
	  server_sock = socket(AF_INET, SOCK_STREAM, 0);
	  if(server_sock < 0)
	  {
	                                                                                                                  
	     perror("[-]Socket Connection is not established!!\n");
                                                                                                            
	     exit(1);
	  }
	  else
	  {
	                                                                                                                  
	     printf("[+]Succesfully Established Socket connection!!\n");
	                                                                                                                  
	  }
	  /* Intializing server_addr by memset takes pointers thats here server_addr is struct type*/
	  memset(&server_addr, '\0', sizeof(server_addr));
	  server_addr.sin_family = AF_INET;
	  server_addr.sin_port   = port;
	  server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	  /*  Binding protocol (sock_connection, struct sockaddr* server_addr, size)   */
	  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	  if(n < 0)
	  {
	                                                                                                                   
	     perror("[-]Binding Connection is not established!!\n");
	                                                                                                                   
	     exit(1);
	  }
	  else
	  {
	                                                                                                                   
	     printf("[+]Succesfully Bind with port num: %d connection!!\n",port);
	                                                                                                                  
	  }
	  
	  /* After connection established we need to listen */
	  listen(server_sock, 5);        
	  printf("Listening...........\n");     
	  addr_size = sizeof(client_addr);                                                                               
	  client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);   //This function syntax is imp                           
	  printf("[+]Client Connected!!\n\n");		                                   
	  
	  /*Server responding to clients*/                                                                                                                
	 
	  
	  return 0;
  }

