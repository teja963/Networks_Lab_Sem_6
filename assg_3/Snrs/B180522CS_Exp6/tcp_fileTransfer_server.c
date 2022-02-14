#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<unistd.h>  
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>     
#include<arpa/inet.h>
#include<pthread.h>

#define PORT 7008
#define IP_ADDR "127.0.0.1"
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

//generate random string
void random_str(char *string)
{
	static const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz!@#$%^&*~?><";

	int n = sizeof(str) - 1;

	for(int i=0;i<10;i++)
		string[i] = str[rand() % n];
}

//communicate with clients
void * communicate(void * Client_details)
{
	struct client* client = (struct client *) Client_details;
	int socketID = client->socketID;
	int index = client->index;
	char name[256];
	strcpy(name, client->name);
	printf("%s connected to server\n", name);
	
	while(1)
	{
		char buffer[500];
		char message[500];
		char string[10];

		//receive messages from client
		int bytes = recv(socketID, buffer, sizeof(buffer), 0);
		buffer[bytes] = '\0';

		if(strcmp(buffer, "Bye")==0)
		{
			sprintf(message, "%s disconnected from server\n", name);
			printf("%s",message);
			break;
		}
		else if(strcmp(buffer, "GivemeyourVideo")==0)  //file transfer
		{
			sprintf(buffer, "SendingVideo\n");
			send(socketID, &buffer, sizeof(buffer), 0);
			FILE *fp;
			fp = fopen("sampleVideo.mp4", "r");

			char msg[500];
			char buf[10];
			int i=0;
			int bytes;
			printf("Initiating file transfer\n");
			//while(fgets(msg, 500, fp)!=NULL)
			while((bytes = fread(msg, 1, 500, fp)) >= 1)
			{
				send(socketID, &msg, bytes,0);
				//confirmation
				recv(socketID, &buf, 10, 0);
				i+=500;
				if(i%5000000==0)
					printf("%d mb sent\n", i/1000000);
			}

			sprintf(msg, "file transfer finished\n");
			send(socketID, &msg, sizeof(msg), 0);
			printf("%s", msg);
		}
		else
		{
			random_str(string);
			send(socketID, &string, sizeof(string), 0);
		}

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
