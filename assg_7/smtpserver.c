#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
static char uname[1024], upassword[1024], search[1024];
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5000;
	  struct sockaddr_in server_addr, client_addr;
	  socklen_t  addr_size;
	  int n;
	  char bf1[1024], bf2[1024];
	  /*   socket connection protocol(domain, type, protocol)   */
	  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
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
	  int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);   //This function syntax is imp                           
	  printf("[+]Client Connected!!\n\n");		                                   
	  
	  
	  /*Server responding to clients*/
	  int flag = 0;
	  while(1){
	  	char tmp1[1024], tmp2[1024];
	  	bzero(bf1, 1024);
	  	recv(client_sock, bf1, sizeof(bf1), 0);
	  	
	  	sscanf(bf1, "%s %*s", tmp1);
	  	sscanf(bf1, "%*s %s", tmp2);
	  	FILE *fp = fopen("logincred.txt", "r");
	  	while(fscanf(fp, "%s %s",search, bf2) != EOF){
	  		if(strcmp(search, tmp1) == 0){
	  			strcpy(uname, search);
	  			strcpy(upassword, bf2);
	  			flag = 1;
	  		}
	  	}
	  	bzero(bf1, 1024);
	  	if(flag == 1){
	  		if(strcmp(upassword, tmp2) == 0){
	  			sprintf(bf1, "Username and Password Verified\n");
	  			send(client_sock, bf1, strlen(bf1), 0);
	  			
	  			bzero(bf1, 1024);
	  			recv(client_sock, bf1, sizeof(bf1), 0);
	  			printf("%s", bf1);
	  			
	  			
	  		
	  		}
	  		else{
	  			sprintf(bf1, "Please Enter Correct Password\n");
	  			send(client_sock, bf1, strlen(bf1), 0);
	  		}
	  	}
	  	else{
	  		sprintf(bf1, "Please Enter Valid Username and Password!!!!!\n");
	  		send(client_sock, bf1, strlen(bf1), 0);
	  			
	  		
	  	} 
	  
	  }
	  return 0;
	       
}
