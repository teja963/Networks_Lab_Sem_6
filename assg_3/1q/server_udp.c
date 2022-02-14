#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
char tmp[1024];
void fun(int num)
{
  	sprintf(tmp, " %d\n", num);
}

void main(int argc, char ** argv){ //Taking port num as command line for reliable connection
	if(argc != 2){
		printf("Usage: %s  port\n", argv[0]);
		exit(0);
	}
	int port = atoi(argv[1]);
	int server_sock;
	struct sockaddr_in server_addr, client_addr;
	char bf1[1024], bf2[1024];
	socklen_t addr_size;
	char fts[][50] = {"apple", "mango", "banana", "chikoo", "papaya"};
	int count[5] = {100, 100, 100, 100, 100};
	
	server_sock = socket(AF_INET, SOCK_DGRAM, 0); //Diff
	
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	addr_size = sizeof(client_addr);
        
        while(1)
	  {                                                                        
	    bzero(bf1, 1024);
	    recvfrom(server_sock, bf1, sizeof(bf1), 0, (struct sockaddr*)&client_addr, &addr_size);
	    printf("Client: %s\n", bf1);
	    if(strcmp(bf1, "Fruits") == 0){
		    
		    bzero(bf1, 1024);
		    strcpy(bf1, "Enter Fruits: ");
		    sendto(server_sock, bf1, strlen(bf1), 0, (struct sockaddr*)&client_addr, addr_size);
		    
		    bzero(bf1, 1024);
		    recvfrom(server_sock, bf1, sizeof(bf1), 0, (struct sockaddr*)&client_addr, &addr_size);
		    
		    bzero(bf2, 1024);
		    strcpy(bf2, "Num of Fruits: ");
		    sendto(server_sock, bf2, strlen(bf2), 0, (struct sockaddr*)&client_addr, addr_size);
		     
		    bzero(bf2, 1024);
		    recvfrom(server_sock, bf2, sizeof(bf2), 0, (struct sockaddr*)&client_addr, &addr_size);  
		    
		    printf("A Request of %s %s from client\n", bf2, bf1);
		    int found  = -1;
		    for(int i = 0; i < 5; i++)
		    {
		      	if(strcmp(bf1, fts[i]) == 0)
		      	{
		      	  	int num = atoi(bf2);
		      	  	if(count[i] >= num)
		      	  	{
		      	  		count[i] -= num;
		      	  		printf("Successfully Updated in database from %d to %d\n", count[i] + num, count[i]);
		      	  		bzero(bf1, 1024);
		      	  		bzero(bf2, 1024);
		      	  		fun(count[i]);
		      	  		strcpy(bf2, tmp);
		      	  		strcpy(bf1, "No. of fruits available is:");
		      	  		strcat(bf1, bf2);
		      	  		
		      	  		sendto(server_sock, bf1, strlen(bf1), 0, (struct sockaddr*)&client_addr, addr_size);
		      	  	}
		      	  	else
		      	  	{
		      	  	   	bzero(bf1, 1024);
		      	  	   	strcpy(bf1, "Fruits are unavailable with count of ");
		      	  	   	strcat(bf1, bf2);
		      	  	   	sendto(server_sock, bf1, strlen(bf1), 0, (struct sockaddr*)&client_addr, addr_size);
		      	  	}
			        found = i;
		      	}
		    }
		    if(found == -1)
		    {
		       	bzero(bf1, 1024);
		       	strcpy(bf1, "Please Enter the valid fruit name!!!");
		       	sendto(server_sock, bf1, strlen(bf1), 0, (struct sockaddr*)&client_addr, addr_size);
		    } 
	    }
	    else if(strcmp(bf1, "SendInventory") == 0)
	    {
	    		sendto(server_sock, "5", 2, 0, (struct sockaddr*)&client_addr, addr_size);
	    		
	    		bzero(bf2, 1024);
	    		recvfrom(server_sock, bf2, sizeof(bf2), 0, (struct sockaddr*)&client_addr, &addr_size); 
	    		if(strcmp(bf2, "1") == 0)
	    		{
	    		        printf("Sending InFormation to Client ");
		    		for(int i = 0; i < 5; i++)
		    		{
                                       char combo[1024];		    		
		    			bzero(bf1, 1024);
		    			bzero(combo, 1024);
		    			fun(count[i]);
		    			strcat(combo, fts[i]);
		    			strcat(combo, tmp);
		    			strcpy(bf1, combo);
		    			sendto(server_sock, bf1, strlen(bf1), 0, (struct sockaddr*)&client_addr, addr_size);
		    			printf(" . . .");
		    		}
		    		printf("\nSent Information Successfully to Client!!\n\n");
		    	}
	    }
		                                                                   
	  }
}
	  
