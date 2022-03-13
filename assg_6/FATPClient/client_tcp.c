#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
char http_status_300[1024] = "Correct Username; Need password", http_status_301[1024] = "Incorrect Username", http_status_305[1024] = "User Authenticated with password", http_status_310[1024] = "Incorrect password", http_status_505[1024] = "Command not supported", http_ok[10] = "Send";
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
			  			
			  			if(strcmp(bf1, "QUIT\n") == 0){
			  				break;
			  				close(sock);
			  			}
			  			
			  			bzero(bf1, 1024);
			  			recv(sock, bf1, sizeof(bf1), 0);
			  			printf("%s\n", bf1);
			  			
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
	  	else{
	  		printf("Please Enter username correctly!!\n");
	  		continue;
	  	}
	  	
	  }
	  return 0;
}
