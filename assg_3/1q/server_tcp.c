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

int main(){
	  char* ip_addr = "127.0.0.1";
	  int port = 5001;
	  int server_sock, client_sock;
	  struct sockaddr_in server_addr, client_addr;
	  socklen_t  addr_size;
	  int n;
	  char fts[][50] = {"apple", "mango", "banana", "chikoo", "papaya"};
	  int count[5] = {100, 100, 100, 100, 100};
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
	  while(1)
	  {                                                                        
		    bzero(bf1, 1024);
		    recv(client_sock, bf1, sizeof(bf1), 0);
		    //printf("%s\n", bf1);
		    if(strcmp(bf1, "Fruits") == 0){
			    
			    bzero(bf1, 1024);
			    strcpy(bf1, "Enter Fruits: ");
			    send(client_sock, bf1, strlen(bf1), 0);
			    
			    bzero(bf1, 1024);
			    recv(client_sock, bf1, sizeof(bf1), 0);
			    
			    bzero(bf2, 1024);
			    strcpy(bf2, "Num of Fruits: ");
			    send(client_sock, bf2, strlen(bf2), 0);
			     
			    bzero(bf2, 1024);
			    recv(client_sock, bf2, sizeof(bf2), 0);  
			    
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
			      	  		
			      	  		send(client_sock, bf1, strlen(bf1), 0);
			      	  	}
			      	  	else
			      	  	{
			      	  	   	bzero(bf1, 1024);
			      	  	   	strcpy(bf1, "Fruits are unavailable with count of ");
			      	  	   	strcat(bf1, bf2);
			      	  	   	send(client_sock, bf1, strlen(bf1), 0);
			      	  	}
				        found = i;
			      	}
			    }
			    if(found == -1)
			    {
			       	bzero(bf1, 1024);
			       	strcpy(bf1, "Please Enter the valid fruit name!!!");
			       	send(client_sock, bf1, strlen(bf1), 0);
			    } 
		    }
		    else if(strcmp(bf1, "SendInventory") == 0){
		    		printf("Sending Data . .");
		    		
		    		bzero(bf1, 1024);
		    		for(int i = 0; i < 5; i++){
		    			char combo[1024];
		    			printf(" . .");
		    			bzero(combo, 1024);
		    			strcat(combo, fts[i]);
		    			fun(count[i]);
		    			
		    			strcat(combo, tmp);
		    			strcat(bf1, combo);
		    		}
		    		send(client_sock, bf1, strlen(bf1), 0);
		    		printf("\nSuccessfully sent the data!!\n\n");
		    }
		    else if(strcmp(bf1, "Terminate") == 0){
		    		close(client_sock);
		    		printf("[-]Client Terminated from the server\n");
		    		break;
		    }
		                                                                   
	  }
	  
	  return 0;
  }

