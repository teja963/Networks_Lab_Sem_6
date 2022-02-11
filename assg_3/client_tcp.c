#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5000;
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
	  while(1){
		  int num;
		  printf("Press 1 : Fruits\tPress 2: Inventory\tPress 3 : Terminate\n");
		  scanf("%d", &num);
		  if(num == 1)
		  {
		       	   bzero(bf1, 1024);
		       	   strcpy(bf1, "Fruits");
		       	   send(sock, bf1, strlen(bf1), 0);
		       	   
		       	   bzero(bf1, 1024);
		       	   recv(sock, bf1, sizeof(bf1), 0);
		       	   printf("%s\n", bf1);
		       	   
		       	   bzero(bf1, 1024);
		       	   scanf("%s", bf1);
		       	   send(sock, bf1, strlen(bf1), 0);
		       	   
		       	   bzero(bf2, 1024);
		       	   recv(sock, bf2, sizeof(bf2), 0);
		       	   printf("%s\n", bf2);
		       	   
		       	   bzero(bf2, 1024);
		       	   scanf("%s", bf2);
		       	   send(sock, bf2, strlen(bf2), 0);
		       	   
		       	   //printf("Succesfully sent to server\n");
		       	   
		       	   bzero(bf1, 1024);
		       	   recv(sock, bf1, sizeof(bf1), 0);
		       	   printf("Server: %s\n", bf1);
		 	    
		  }
		  else if(num == 2)
		  {		
		  	  bzero(bf1, 1024);
		  	  strcpy(bf1, "SendInventory");
		  	  send(sock, bf1, strlen(bf1), 0);
		  	  //printf("Asked Details to Server\n");
		  	  
		  	  bzero(bf2, 1024);
		  	  recv(sock, bf2, sizeof(bf2), 0);
		  	  int count = atoi(bf2);
		  	  
		  	  bzero(bf2, 1024);
		  	  send(sock, "1", 2, 0);
		  	  printf("Receiving starts from server.........\n");
		  	  
		  	  while(count--){
		  	  	bzero(bf1, 1024);
		  	  	recv(sock, bf1, sizeof(bf1), 0);
		  	  	printf("%s", bf1);
		  	  }
		  	  printf("\n");
		  	  
		  }  
		  else if(num == 3)
		  {
		           bzero(bf1, 1024);
		           strcpy(bf1, "Terminate");
		           send(sock, bf1, strlen(bf1), 0);
		 	   close(sock);                                                                                     
			   printf("[-]Client Disconnected!!!!\n");
		           break;  
	          }                        
	  }
	  return 0;
	  }
