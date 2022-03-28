#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8000
#define PORT_1 8001
static char uname[1024], upassword[1024];
char tmp[1024];
char *ip_addr = "127.0.0.1";
struct sockaddr_in addr;
void calendar(char curr[], int sz){
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    strftime(curr, sz, "%D %T", localtime(&t.tv_sec));
}
int fun_socket(int port){
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
	  return sock;
}

int main(){
	  socklen_t  addr_size;
	  int n, sock, sock_pop;
	  char bf1[1024], bf2[1024];
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
	  addr.sin_port   = PORT;
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
		 
		 send(sock, bf2, strlen(bf2), 0);
		 
		 bzero(bf1, 1024);
		 recv(sock, bf1, sizeof(bf1), 0);
		 if(strcmp(bf1, "Verified\n") == 0){
		 	strcpy(uname, tmp);
		 	strcpy(upassword, bf1);
		 }
		 
		 if(strcmp(bf1, "Verified\n") == 0){
		 	while(1){
		 		sprintf(bf1, "goodbye!!");
		 		send(sock, bf1, strlen(bf1), 0);
		 		close(sock);
		 		
		 		printf("\n\t\tPress 1: Manage Mail\n\t\tPress 2: Send Mail\n\t\tPress 3: Quit\n\n");
			 	printf("Enter your option: ");
			 	fgets(tmp, 1024, stdin);
			 	if(strcmp(tmp, "1\n") == 0){
			 		sock_pop = fun_socket(PORT_1);
			 		
			 		check = connect(sock_pop, (struct sockaddr*)&addr, sizeof(addr));
	  				if(check != 0){
	  				         perror("Connection not established\n");
	  					 exit(1);
	  				}
	  				else printf("[+]200 Ok - Connected to server\n\n");
	  				
	  				bzero(bf1, 1024);
	  				strcat(bf1, uname);
	  				strcat(bf1, " ");
	  				strcat(bf1, upassword);
	  				send(sock_pop, bf1, strlen(bf1), 0);
	  				
	  				printf("Enter the operations to perform\n");
	  				char ch[3];
	  				fgets(ch, 3, stdin);
	  				if(strcmp(ch, "q\n") == 0){
	  					sprintf(bf1, "goodbye");
	  					send(sock_pop, bf1, strlen(bf1), 0);
	  					
	  					bzero(bf1, 1024);
	  					recv(sock_pop, bf1, sizeof(bf1), 0);
	  					printf("%s\n", bf1);	
	  				}
	  				else if(strcmp(ch, "d\n") == 0){
	  					fgets(bf1, 1024, stdin);
	  					send(sock_pop, bf1, strlen(bf1), 0);
	  					
	  					bzero(bf1, 1024);
	  					recv(sock_pop, bf1, sizeof(bf1), 0);
	  					printf("%s", bf1);
	  				}
	  				else if(strcmp(ch, "s\n") == 0){
	  					sprintf(bf1, "show");
	  					printf("\n");
	  					send(sock_pop, bf1, strlen(bf1), 0);
	  					
	  					bzero(bf1, 1024);
	  					recv(sock_pop, bf1, sizeof(bf1), 0);
	  					printf("%s", bf1);
	  				}
			 	}
			 	else if(strcmp(tmp, "2\n") == 0){
			 		sock = fun_socket(PORT);
			 		
			 		check = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	  				if(check != 0){
	  				         perror("Connection not established\n");
	  					 exit(1);
	  				}
	  				else printf("[+]200 Ok - Connected to server\n\n");
	  				
	  				//Password sending again
	  				bzero(bf2, 1024);
	  				strcat(bf2, uname);
	  				strcat(bf2, upassword);
					send(sock, bf2, strlen(bf2), 0);
					 
					bzero(bf1, 1024);
					recv(sock, bf1, sizeof(bf1), 0);
					
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
				 	fgets(tmp, 1024, stdin);
				 	strcat(bf1, tmp);
				        
				        char time[50];
				 	calendar(time, 50);
				 	strcat(bf1, "RECEIVED TIME: ");
				 	strcat(bf1, time);
				 	strcat(bf1, "\n");
				        
				        char ch;
				 	int i = 0;
				 	printf("Body: ");
				 	strcat(bf1, "Body: ");
				 	bzero(tmp, 1024);
				 	while(ch != '.'){
				 		ch = getchar();
				 		tmp[i++] = ch;
				 	}
				 	getchar();        //after dot we are pressing dot character
				 	strcat(bf1, tmp);
				 	strcat(bf1, "\n");
				 	
				 	send(sock, bf1, strlen(bf1), 0);
				 	
				 	bzero(bf1, 1024);
				 	recv(sock, bf1, sizeof(bf1), 0);
				 	printf("%s", bf1);
				 }
				 else if(strcmp(tmp, "3\n") == 0){
				 	sprintf(bf1, "goodbye!!");
				 	send(sock, bf1, strlen(bf1), 0);
				 	break;
				 }
		 	}		 	
		 }
		 close(sock);
		 break; 	
	  }
	  return 0;
}
