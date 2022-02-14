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
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);	
	char *response = "HTTP/1.1 200 OK\r\nServer: Singam\r\nContent-Length: 74\r\nContent-type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Welcome to Networks Lab!</h1></body></html>";

	//define server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);	
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	//bind the socket to specified IP and port
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	//listen to connections
	listen(server_socket, 5);


	while(1)
	{
		//accept connections
		int client_socket = accept(server_socket, NULL, NULL);
		printf("New Client connected\n\n");

		//receiving request
		char buff[500];
		int bytes = recv(client_socket, &buff, sizeof(buff), 0);
		buff[bytes] = '\0';
		printf("Http request message received\n\n%s\n", buff);

		//sending response
		char server_message[256];
		strcpy(server_message, response);
		send(client_socket, server_message, sizeof(server_message), 0);
		printf("Http response sent\n\n");

		printf("Connection closed\n\n");
		close(client_socket);

	}
	//closing socket
	close(server_socket);

	return 0;
}
