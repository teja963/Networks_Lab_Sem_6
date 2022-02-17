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

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int sockID, flag=1;

//thread to send messages to server
void * sender()
{
	while(1)
	{
		char msg[256];
		scanf(" %[^\n]%*c", msg);
		send(sockID, msg, sizeof(msg), 0);
		if(strcmp(msg, "tata")==0)
			break;
	}
	flag=0;
	printf("--You left the chat--\n");
	close(sockID);
}

//thread to receive messages from server
void * receiver()
{
	while(flag==1)
	{
		char msg[256];
		if(recv(sockID, msg, sizeof(msg), 0) > 0)
		{
			printf("%s", msg);
		}
	}
}

int main()
{
	//create a socket
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(client_socket < 0)
	{
		printf("Error in connection\n");
		exit(1);
	}
	
	//specify an address for socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP_ADDR);

	//Establish a connection using connect()
	int connect_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connect_status < 0)
	{
		printf("Connection failed\n");
		exit(1);
	}
	
	printf("**********************************\n");
	printf("Welcome to Group Chat!!!\n");
	printf("Rules\n");
	printf("1.Enter your name to start\n");
	printf("2.Enter tata to leave chat\n");
	printf("3.Be polite to everyone\n");
	printf("**********************************\n");

	char name[256];
	printf("Your name: ");
	scanf("%[^\n]%*c",name);
	send(client_socket, name, sizeof(name), 0);
	printf("--You joined the chat--\n");

	sockID = client_socket;

	//sender thread
	pthread_t sender_thread;
	if(pthread_create(&sender_thread, NULL, &sender, NULL)!=0)
		printf("Error in creating sender thread\n");
	
	//receiver thread
	pthread_t receiver_thread;
	if(pthread_create(&receiver_thread, NULL, &receiver, NULL)!=0)
		printf("Error in creating receiver thread\n");

	//join threads
	pthread_join(sender_thread, 0);
	pthread_join(receiver_thread, 0);

	return 0;
}
