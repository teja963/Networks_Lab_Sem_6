#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<unistd.h>  
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>     
#include<arpa/inet.h>
#include<pthread.h>

#define PORT 7007
#define IP_ADDR "192.168.2.6"
#define MAX_CLIENTS 256

static int client_count=0;

struct client{
	int index;
	int socketID;
	char name[256];
};

//mutex to synchronize betweeen threads
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct client *Client[256];
pthread_t thread[256];

//store clients
void store_client(struct client *client)
{
	pthread_mutex_lock(&mutex);

	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(!Client[i]){
			Client[i] = client;
			break;
		}
	}

	pthread_mutex_unlock(&mutex);
}

//remove clients
void remove_client(int index)
{
	pthread_mutex_lock(&mutex);

	for(int i=0;i<MAX_CLIENTS;i++)
	{
		if(Client[i]){
			if(Client[i]->index==index)
			{
				Client[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&mutex);
}

//send message to all clients except sender
void send_msg(char buffer[], int index)
{
	pthread_mutex_lock(&mutex);

	for(int i=0;i<MAX_CLIENTS;i++)
		if(Client[i])
			if(Client[i]->index!=index)
			{
				send(Client[i]->socketID, buffer, 256, 0);
			}

	pthread_mutex_unlock(&mutex);
}

//communicate with clients
void * communicate(void * Client_details)
{
	struct client* client = (struct client *) Client_details;
	int socketID = client->socketID;
	int index = client->index;
	char name[256];
	strcpy(name, client->name);
	char buffer[256];

	//send online usernames
	sprintf(buffer, "Currently online users\n");
	send(socketID, buffer, sizeof(buffer), 0);
	int active=0;
	for(int i=0;i<MAX_CLIENTS;i++)
		if(Client[i])
		{
			active++;
			if(Client[i]->index!=index)
			{
				char buffer[256];
				sprintf(buffer, "-%s\n", Client[i]->name);
				send(socketID, buffer, sizeof(buffer), 0);
			}
		}
	
	if(active==1)
	{
		sprintf(buffer, "No users online\n******* Please wait *******\n");
		send(socketID, buffer, sizeof(buffer), 0);
	}
	else
	{
		sprintf(buffer, "******* Say hi *******\n");
		send(socketID, buffer, sizeof(buffer), 0);
	}
	
	sprintf(buffer,"--%s has joined the chat--\n", name);
	printf("%s", buffer);
	send_msg(buffer, index);

	while(1)
	{
		char buffer[256];
		char message[256];

		//receive messages from client
		int bytes = recv(socketID, buffer, sizeof(buffer), 0);
		buffer[bytes] = '\0';

		if(strcmp(buffer, "tata")==0)
		{
			sprintf(message, "--%s has left the chat--\n", name);
			send_msg(message, index);
			printf("%s",message);
			break;
		}

		sprintf(message, ">%s: %s\n", name, buffer);
		send_msg(message, index);
		printf("%s",message);
	}

	//remove client
	close(socketID);
	remove_client(index);
  	free(client);
  	client_count--;
  	pthread_detach(pthread_self());

	return NULL;
}
	
int main() 
{
	// create a server socket
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket < 0)
	{
		printf("Error in connection\n");
		exit(1);
	}
	printf("Server socket created\n");

	//define server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP_ADDR);

	//bind the socket to specified IP and port
	int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(bind_status < 0)
	{
		printf("Error in binding\n");
		exit(1);
	}
	printf("Binded to port %d\n", PORT);

	//listen to connections
	int listen_status = listen(server_socket, 5);
	if(listen_status==0)
		printf("Listening\n");
	else
	{
		printf("Error in binding\n");
		exit(1);
	}
	printf("***********************\n");

	//accept connections 
	while(1)
	{
		int socketID = accept(server_socket, NULL, NULL);
		if(client_count+1 == MAX_CLIENTS)
		{
			close(socketID);
			continue;
		}

		struct client *client_details = (struct client *)malloc(sizeof(struct client));
		client_details->socketID = socketID;
		client_details->index = client_count;

		char name[256];
		int bytes = recv(socketID, name, sizeof(name), 0);
		name[bytes] = '\0';
		strcpy(client_details->name, name);

		//store client in structure array
		store_client(client_details);

		//thread to handle clients
		pthread_create(&thread[client_count], NULL, &communicate, (void *) client_details);

		client_count++;
	}

	return 0;
}
