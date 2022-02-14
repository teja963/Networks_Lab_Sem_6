#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>

void main(int argc, char ** argv){ //Taking port num as command line for reliable connection
	if(argc != 2){
		printf("Usage: %s  port\n", argv[0]);
		exit(0);
	}
	int port = atoi(argv[1]);
	int sock;
	struct sockaddr_in addr;
	char bf1[1024], bf2[1024];
	socklen_t addr_size;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0); //Diff
	
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr_size = sizeof(addr);
	
	while(1){
	        printf("Press 1: Fruits\t Press 2: SendInventory\t Press 3: Terminate\n");
	        int num;
	        scanf("%d", &num);
	        if(num == 1){
			bzero(bf1, 1024);
			strcpy(bf1, "Fruits");
			sendto(sock, bf1, strlen(bf1), 0, (struct sockaddr*)&addr, addr_size);
			
			bzero(bf1, 1024);
			recvfrom(sock, bf1, sizeof(bf1), 0, (struct sockaddr*)&addr, &addr_size);	
			printf("From Server %s\n", bf1);
			
			bzero(bf1, 1024);
			scanf("%s", bf1);
			sendto(sock, bf1, strlen(bf1), 0, (struct sockaddr*)&addr, addr_size);
			
			bzero(bf2, 1024);
			recvfrom(sock, bf2, sizeof(bf2), 0, (struct sockaddr*)&addr, &addr_size);	
			printf("Server: %s\n", bf2);
			
			bzero(bf2, 1024);
			scanf("%s", bf2);
			sendto(sock, bf2, strlen(bf2), 0, (struct sockaddr*)&addr, addr_size);
			
			printf("Receiving from client.....\n");
			
			bzero(bf1, 1024);
			recvfrom(sock, bf1, sizeof(bf1), 0, (struct sockaddr*)&addr, &addr_size);	
			printf("From Server %s\n", bf1);
						
		}else if(num == 2){		
		  	  bzero(bf1, 1024);
		  	  strcpy(bf1, "SendInventory");
		  	  sendto(sock, bf1, strlen(bf1), 0, (struct sockaddr*)&addr, addr_size);
		  	  printf("Asked Details to Server\n");
		  	  
		  	  bzero(bf2, 1024);
		  	  recvfrom(sock, bf2, sizeof(bf2), 0, (struct sockaddr*)&addr, &addr_size);
		  	  int count = atoi(bf2);
		  	  
		  	 
		  	  sendto(sock, "1", 2, 0, (struct sockaddr*)&addr, addr_size);
		  	  printf("Receiving starts from server.........\n");
		  	  
		  	  while(count--){
		  	  	bzero(bf1, 1024);
		  	  	recvfrom(sock, bf1, sizeof(bf1), 0, (struct sockaddr*)&addr, &addr_size);
		  	  	printf("%s", bf1);
		  	  }
		  	  printf("\n");
		  	  
		  }  
		  else if(num == 3)
		  {
		           bzero(bf1, 1024);
		           strcpy(bf1, "Terminate");
		           sendto(sock, bf1, strlen(bf1), 0, (struct sockaddr*)&addr, addr_size);
		 	   close(sock);                                                                                     
			   printf("[-]Client Disconnected!!!!\n");
		           break;  
	          }       
	}
}
	  
