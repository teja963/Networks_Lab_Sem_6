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
	  int port = 5000;
	  struct sockaddr_in addr;
	  socklen_t  addr_size;
	  int n;
	  char bf1[1024], bf2[1024];
	  /*   socket connection protocol(domain, type, protocol)   */
	  int sock = socket(AF_INET, SOCK_STREAM, 0);
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
	  
	  int check = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	  if(check != 0){
	  	perror("Connection not established\n");
	  	exit(1);
	  }
	  else printf("[+]200 Ok - Connected to server\n\n");
	  
	  /*Client request server starts*/
	  while(1){
		 printf("Enter the Username:");
		 fgets(tmp, 1024, stdin);
		 //printf("%ld\n", strlen(tmp));
		 printf("Enter the Password:");
		 fgets(bf1, 1024, stdin);
		 
		 bzero(bf2, 1024);
		 tmp[strlen(tmp) - 1] = '\0';
		 bf1[strlen(bf1) - 1] = '\0';
		 strcat(bf2, tmp);
		 strcat(bf2, " ");
		 strcat(bf2, bf1);
		 //printf("%ld\n", strlen(bf2));
		 //printf("%s\n", bf2);
		 send(sock, bf2, strlen(bf2), 0);
		 
		 bzero(bf1, 1024);
		 recv(sock, bf1, sizeof(bf1), 0);
		 printf("%s", bf1);
		 
		 if(strcmp(bf1, "Username and Password Verified\n") == 0){
		 	while(1){
		 		printf("\t\tPress 1: Manage Mail\n\t\tPress 2: Send Mail\n\t\tPress 3: Quit\n\n");
			 	printf("Enter your option: ");
			 	fgets(tmp, 1024, stdin);
			 	
			 	if(strcmp(tmp, "2\n") == 0){
			 		printf("To end the body Press '.' character\n");
				 	bzero(bf1, 1024);
				 	
				 	printf("To: ");
				 	strcat(bf1, "To: ");
				 	fgets(tmp, 1024, stdin);
				 	strcat(bf1, tmp);
				 	
				 	printf("From: ");
				 	strcat(bf1, "From: ");
				 	fgets(tmp, 1024, stdin);
				 	strcat(bf1, tmp);
				 	
				 	printf("Subject: ");
				 	strcat(bf1, "Subject: ");
				 	fgets(tmp, 51, stdin);
				 	strcat(bf1, tmp);
				        
				        char ch;
				 	int i = 0;
				 	printf("Body: ");
				 	strcat(bf1, "Body: ");
				 	bzero(tmp, 1024);
				 	while(ch != '.'){
				 		ch = getchar();
				 		tmp[i++] = ch;
				 	}
				 	strcat(bf1, tmp);
				 	strcat(bf1, "\n.");
				 	
				 	send(sock, bf1, strlen(bf1), 0);
				 }
				 else if(strcmp(tmp, "3\n") == 0){
				 	printf("goodbye\n");
				 	close(sock);
				 	break;
				 }
		 	}		 	
		 }
		 break; 	
	  }
}
