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
int client_sock;
static int check = 0;
static char uname[1024], upassword[1024], search[1024];
FILE *fp;
char bf1[1024], bf2[1024];
void display(){
	int i = 0, j = 1;
	char ch, tmp[10];
	
	bzero(bf1, 1024);
	bf1[i++] = '1';
	bf1[i++] = '.';
	bf1[i++] = '\n';
	
	while((ch = fgetc(fp)) != EOF){
		if(ch == '.'){
			if((ch = fgetc(fp)) == '\n'){
  				j++;
  				bf1[i++] = '\n';
  				bf1[i++] = j + '0';
  				bf1[i++] = '.';
  				bf1[i++] = '\n'; 					
			}
			else{
				bf1[i++] = '.';
				bf1[i++] = ch;
			}
		}
		else{
			bf1[i++] = ch; 
		}	
	}
	bf1[i-4] = '\0';
}

int main(){
	  char* ip_addr = "127.0.0.1";
	  int port;
	  printf("Enter the port number: ");
	  scanf("%d",&port); 
	  struct sockaddr_in server_addr, client_addr;
	  socklen_t  addr_size;
	  int n;
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
	  
	  
	  /*Server responding to clients*/
	  while(1){
	  	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);   //This function syntax is imp                           
	  	printf("[+]Client Connected!!\n\n");
	  	
	  	bzero(bf1, 1024);
	  	recv(client_sock, bf1, sizeof(bf1), 0);
	  	sscanf(bf1, "%s %*s", uname);
	  	sscanf(bf1, "%*s %s", upassword);
	  	strcat(uname, "/mymailbox");
	  	
	  	fp = fopen(uname, "r");
	  	
	  	if(fp == NULL){
	  		printf("file can't be opened\n");
	  	}
	  	
	  	bzero(bf1, 1024);
	  	recv(client_sock, bf1, sizeof(bf1), 0);
	  	
	  	if(strcmp(bf1, "goodbye") == 0){
	  		sprintf(bf1, "goodbye");
	  		send(client_sock, bf1, strlen(bf1), 0);
	  		break;
	  	}
	  	else if(atoi(bf1) >= 1){
	  		int i = 0, j = 1, num = atoi(bf1);
	  		char ch, tmp[10];
	  		printf("Client Asking to delete %d mail\n", num);
	  		
	  		bzero(bf1, 1024);
	  		while((ch = fgetc(fp)) != EOF){
	  			if(ch == '.'){
	  				ch = fgetc(fp);
	  				if(ch == '\n' ){
	  					j++;
	  					if(j != num){
		  					bf1[i++] = '.';
		  					bf1[i++] = '\n';   						
	  					}
	  				}
	  				else if(j != num){
	  					bf1[i++] = '.';
	  					bf1[i++] = ch;
	  				}
	  			}
	  			else{
	  				if(j != num)bf1[i++] = ch; 
	  			}	
	  		}
	  		bf1[i-4] = '\0';
	  		
	  		FILE *ptr = fopen(uname, "w+");
	  		if(ptr == NULL){
	  			printf("Error!");
	  			exit(1);
	  		}
	  		fprintf(ptr, "%s", bf1);
	  		fclose(ptr);
	  		
	  		sprintf(bf1, "Data Updated Successfully!!\n");
	  		send(client_sock, bf1, strlen(bf1), 0);
		  				
	  	}
	  	else if(strcmp(bf1, "show") == 0){
	  		printf("Client Asking list\n");
                        display();	  
	  
	  		send(client_sock, bf1, strlen(bf1), 0);
	  		printf("Data Sent!!!\n");
	  			
	  	}
	  	fclose(fp);	
	  }
	  return 0;
	       
}
