#include<stdio.h> 
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>         //libraries for definitions of socket functions
#include<sys/socket.h>
#include<unistd.h>

#include<netinet/in.h>        //contains structures needed to store addresses
#include<arpa/inet.h>

int main() 
{
	// create server socket
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);		//socket(communication domain, communication type, protocol) returns descriptor

	//define server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(6000);				//The htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order.
	server_address.sin_addr.s_addr = inet_addr("192.168.2.6"); 	//The inet_addr() function converts the Internet host address cp from IPv4 numbers-and-dots notation into binary data in network byte order.

	//bind the socket to specified IP and port
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	//listen to connections
	listen(server_socket, 5);

	//accept connections
	int client_socket = accept(server_socket, NULL, NULL);

	//sending msg
	char server_message[256];
	strcpy(server_message, "Connected to server\n");
	send(client_socket, server_message, sizeof(server_message), 0);

	//closing socket
	close(server_socket);

	return 0;
}
