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
int server_sock, client_sock;

void fun_List(char* bf2){
	bzero(bf2, 1024);
	DIR* dir = opendir(".");  //pointing to current directory
	if(dir){
		struct dirent* entity;  //reading from that directory
		while((entity = readdir(dir)) != NULL){
			strcat(bf2, "\n");
			strcat(bf2, entity->d_name);
			
		}
		closedir(dir);	
	}	
}

void fun_Create(char* name, char* bf2){
	bzero(bf2, 1024);
	
	FILE* fp = fopen(name, "w+");
	
	sprintf(bf2, "File created successfully!!!!\n");
	fclose(fp);
	
}

void fun_Get_File(char* name, char* bf2){
	char bf[500];
	bzero(bf2, 1024);
	sprintf(bf2, "Sending video");
	send(client_sock, bf2, strlen(bf2), 0);
	
	FILE *fp = fopen(name, "r");
	
	printf("File Transfer starting Server!!!\n");
	int bytes = 0;
	while( (bytes = fread(bf2, 1, 1024, fp)) >= 1){
		send(client_sock, bf2, strlen(bf2), 0);
		
		//Conformation
		bzero(bf, 500);
		recv(client_sock, bf, sizeof(bf), 0);
	}
	fclose(fp);
	sprintf(bf2, "Finished");
	send(client_sock, bf2, strlen(bf2), 0);
	sprintf(bf2, "File Received Successfully!!!!!");
}

void fun_Store_File(char* name){
	char bf2[1024];
	bzero(bf2, 1024);
	sprintf(bf2, "Dummy Flow");
	send(client_sock, bf2, strlen(bf2), 0);
	
	if(recv(client_sock, bf2, sizeof(bf2), 0) > 0){
		if(strcmp(bf2, "Sending video") == 0){
			FILE *fp = fopen(name, "w");
			
			bzero(bf2, 1024);
			while(recv(client_sock, bf2, sizeof(bf2), 0) >= 1){
				
				if(strcmp(bf2, "Finished") == 0)break;
				
				//acknowledgement
				char bf[500];
				sprintf(bf,"Received");
				send(client_sock, bf, strlen(bf), 0);
				
				fprintf(fp, "%s", bf2);
				bzero(bf2, 1024);	
			}
			fclose(fp);
			
			bzero(bf2, 1024);
			sprintf(bf2, "Successfully Sent to server!!!");
			send(client_sock, bf2, strlen(bf2), 0);
			printf("%s\n", bf2);
				
		}
		else{
		printf("%s", search);
		fflush(stdout);
		}
	}
}

int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5001;
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
		  		FILE* fp = fopen("../logincred.txt", "r");
		  		
		  		while(fscanf(fp, "%s %s", uname, upass) != EOF){
		  			if(strcmp(uname, search) == 0){
		  				printf("Username %s found!!\n", uname);
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
	  					
	  					client_logined:  /*Client Logined Activity Process*/
	  					
		  					bzero(bf1, 1024);
		  					recv(client_sock, bf1, sizeof(bf1), 0);
		  					bzero(cmd, 1024);
		  					bzero(search, 1024);
		  					sscanf(bf1, "%s %*s", cmd);
		  					sscanf(bf1, "%*s %s", search);
		  					
		  					
		  					if(strcmp(cmd, "ListDir") == 0){
		  						printf("Client Asking Files List!!!!\n");
		  						fun_List(bf2);
		  						send(client_sock, bf2, strlen(bf2), 0);
		  					}
		  					else if(strcmp(cmd, "CreateFile") == 0){
		  						printf("Client requested to create file!!!!\n\n");
		  						fun_Create(search, bf2);
		  						send(client_sock, bf2, strlen(bf2), 0);
		  					}
		  					else if(strcmp(cmd, "StoreFile") == 0){
		  						printf("Receving file %s from client!!!!\n\n", search);
		  						fun_Store_File(search);
		  					}
		  					else if(strcmp(cmd, "GetFile") == 0){
		  						printf("Cliented requested for File!!!!\n\n");
		  						fun_Get_File(search, bf2);
		  						send(client_sock, bf2, strlen(bf2), 0);
		  					}
		  					else if(strcmp(cmd, "QUIT") == 0){
		  						printf("[-]Client Disconnected!!\n");
		  						break;
		  					}
		  					goto client_logined;
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

