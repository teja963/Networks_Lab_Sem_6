#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
char http_status_300[1024] = "300  Correct Username; Need password", http_status_301[1024] = "301  Incorrect Username", http_status_305[1024] = "305  User Authenticated with password", http_status_310[1024] = "310  Incorrect password", http_status_505[1024] = "505  Command not supported", http_ok[10] = "Send";
int sock;
char cmd[1024], search[1024];

void fun_Get_File(char* name){
	bzero(search, 1024);
	if(recv(sock, search, sizeof(search), 0) > 0){
		if(strcmp(search, "Sending video") == 0){
			FILE *fp = fopen(name, "w");
			
			bzero(search, 1024);
			while(recv(sock, search, sizeof(search), 0) >= 1){
				
				if(strcmp(search, "Finished") == 0)break;
				
				//acknowledgement
				char bf[500];
				sprintf(bf,"Received");
				send(sock, bf, strlen(bf), 0);
				
				fprintf(fp, "%s", search);
				bzero(search, 1024);	
			}
			fclose(fp);
				
		}
		else{
		printf("%s", search);
		fflush(stdout);
		}
	}
}

void fun_Store_File(char* name){
	char bf[500];
	bzero(search, 1024);
	recv(sock, search, sizeof(search), 0);
	
	bzero(search, 1024);
	sprintf(search, "Sending video");
	send(sock, search, strlen(search), 0);
	
	FILE *fp = fopen(name, "r");
	
	printf("File Transfer starting Client!!!\n");
	int bytes = 0;
	while( (bytes = fread(search, 1, 1024, fp)) >= 1){
		send(sock, search, strlen(search), 0);
		
		//Conformation
		bzero(bf, 500);
		recv(sock, bf, sizeof(bf), 0);
	}
	fclose(fp);
	sprintf(search, "Finished");
	send(sock, search, strlen(search), 0);
	
}
int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5001;
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
	  
	  //starting connection
	  while(1){
	  	if(fgets(bf1, sizeof(bf1), stdin) == NULL){
	  		perror("Fgets");
	  		exit(0);
	  	}
	  	if(strcmp(bf1, "START\n") == 0){
	  		break;
	  	}
	  	else if(strcmp(bf1, "QUIT\n") == 0){
	  		exit(0);
	  	}
	  }
	  
	  int check = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	  if(check != 0){
	  	perror("Connection not established\n");
	  	exit(1);
	  }
	  else printf("[+]200 Ok - Connected to server\n\n");
	  
	  /*Client request server starts*/
	  printf("Enter UserName and Password!!\n");
	  while(1){
	  	bzero(bf1, 1024);
	  	fgets(bf1, 1024, stdin);
	  	send(sock, bf1, strlen(bf1), 0);
	  	
	  	bzero(bf1, 1024);
	  	recv(sock, bf1, sizeof(bf1), 0);
	  	printf("[Server]: %s\n", bf1);
	  	
	  	
	  	if(strcmp(bf1, http_status_505) == 0){        /* Checking server response about command*/
	  		perror("[-]Enter valid command\n");
	  	}
	  	else if(strcmp(bf1, http_status_300) == 0){   /* Checking server response of username */
	  		password_taking:
		  		bzero(bf1, 1024);
		  		fgets(bf1, 1024, stdin);
		  		send(sock, bf1, strlen(bf1), 0);
		  		
		  		bzero(bf1, 1024);
		  		recv(sock, bf1, sizeof(bf1), 0);
		  		printf("[Server]: %s\n", bf1);
		  		
		  		if(strcmp(bf1, http_status_505) == 0){
		  			perror("[-]Enter valid Command\n");
		  		}
		  		else if(strcmp(bf1, http_status_305) == 0){ /*Checking server response of password*/
		  			send(sock, http_ok, strlen(http_ok), 0);
		  			
		  			bzero(bf1, 1024);
		  			recv(sock, bf1, sizeof(bf1), 0); //getting username from server
		  			printf("Welcome <%s>!!!!\n", bf1);
		  			
		  			client_logined:   /*After logined activity*/
			  			
			  			bzero(bf1, 1024);
			  			fgets(bf1, 1024, stdin);
			  			send(sock, bf1, strlen(bf1), 0);
			  			sscanf(bf1, "%s %*s", cmd);
			  			sscanf(bf1, "%*s %s", bf2);
			  			
			  			if(strcmp(cmd, "StoreFile") == 0){
			  				fun_Store_File(bf2);	
			  			}
			  			if(strcmp(cmd, "GetFile") == 0){
							fun_Get_File(bf2);
		   				}	
			  			else if(strcmp(cmd, "QUIT") == 0){
			  				break;
			  				close(sock);
			  			}
			  			
			  			bzero(bf1, 1024);
			  			recv(sock, bf1, sizeof(bf1), 0);
			  			printf("->%s\n", bf1);
			  			
			  			goto client_logined;
		  			
		  		}
		  		else{
		  			printf("Please Enter Password Correctly!!!\n");
		  			goto password_taking;	
	  		}		
	  	}
	  	else if(strcmp(bf1, "Already Login") == 0){
	  		printf("You Already Logined!!\n");
	  	}
	  	else if(strcmp(bf1, http_status_300) != 0){
	  		printf("Please Enter username correctly!!\n");
	  		continue;
	  	}
	  	
	  }
	  return 0;
}
