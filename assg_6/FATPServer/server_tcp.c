#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <dirent.h>
char cmd[1024], search[1024], http_status_300[1024] = "Correct Username; Need password", http_status_301[1024] = "Incorrect Username", http_status_305[1024] = "User Authenticated with password", http_status_310[1024] = "Incorrect password", http_status_505[1024] = "Command not supported";
static char user_name[1024], password[1024];
static int login = 0;
void fun_List(){
	DIR* dir = opendir(".");  //pointing to current directory
	if(dir == NULL){
		strcpy(search, "NO FILES FOUND!!");
	}
	
	struct dirent* entity = readdir(dir);  //reading from that directory
	while(entity != NULL){
		strcat(search, entity->d_name);
		strcat(search, "\n");
		entity = readdir(dir);
	}
	
	closedir(dir);
}
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
	  while(1){
	  	bzero(bf1, 1024);
	  	recv(client_sock, bf1, sizeof(bf1), 0);
	  	
	  	bzero(cmd, 1024);
	  	bzero(search, 1024);
	  	sscanf(bf1, "%s %*s",cmd);
	  	sscanf(bf1, "%*s %s", search);
	  	
	  	/*USERN function*/
	  	if(strcmp(cmd, "USERN") == 0){
	  		if(login == 0){
	  			bool user_found = 0;
		  		printf("Searching for username %s\n\n", search);
		  		char uname[1024], upass[1024];
		  		 /*File Operations*/
		  		FILE* fp = fopen("/home/user/Documents/Sem 6/NETWORKS-LAB/assg_6/logincred.txt", "r");
		  		
		  		while(fscanf(fp, "%s %s", uname, upass) != EOF){
		  			if(strcmp(uname, search) == 0){
		  				printf("Username found!!\n");
		  				strcpy(user_name, uname);
		  				strcpy(password, upass);
		  				send(client_sock, http_status_300, strlen(http_status_300), 0);
		  				user_found = 1;
		  				login = 1;
		  				break;
		  			}
		  		}
		  		
		  		/*After getting username and password from server*/
		  		if(user_found == 0){
		  			printf("User %s not found!!\n", search);
		  			send(client_sock, http_status_301, strlen(http_status_301), 0);
		  		}
	  		}
	  		else{
	  			bzero(bf1, 1024);
	  			strcpy(bf1, "Already Login");
	  			send(client_sock, bf1, strlen(bf1), 0);
	  		}
	  		
	  	}
	  	else if(strcmp(cmd, "PASSWD") == 0){ /*Taking password and Checking password*/
	  				if(strcmp(search, password) == 0){
	  					send(client_sock, http_status_305, strlen(http_status_305), 0);
	  					
	  					bzero(cmd, 1024);
	  					recv(client_sock, cmd, sizeof(cmd), 0);

	  					send(client_sock, user_name, strlen(user_name), 0);     /*Sending user name*/
	  					
	  					bzero(cmd, 1024);
	  					recv(client_sock, cmd, sizeof(cmd), 0);
	  					
	  					if(strcmp(cmd, "ListDir\n") == 0){
	  						printf("Client Asking Files List!!!!\n");
	  						fun_List();
	  						send(client_sock, search, strlen(search), 0);
	  					}
	  				}
	  				else{
	  					printf("Incorrect Password!!\n");
	  					send(client_sock, http_status_310, strlen(http_status_310), 0);
	  				}	
	  	}  			
	  	else{
	  		send(client_sock, http_status_505, strlen(http_status_505), 0);
	  	} 
	  }
	  return 0;
}

