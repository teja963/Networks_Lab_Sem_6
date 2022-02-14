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
	//create a socket
	int socketid = socket(PF_INET, SOCK_STREAM, 0); 			//socket(communication domain, communication type, protocol) returns descriptor
	
	//specify an address for socket
 	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(6000);                 			//The htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order.
	server_address.sin_addr.s_addr = inet_addr("192.168.2.6");           	//The inet_addr() function converts the Internet host address cp from IPv4 numbers-and-dots notation into binary data in network byte order.

	//Establish connection using connect()
	int connect_status = connect(socketid, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connect_status<0)
		printf("connection failed\n");

	//receiving data
	char server_response[256];
	recv(socketid, server_response, sizeof(server_response), 0);
	printf("server response: %s\n", server_response);

	//close the socket
	close(socketid);
	
	return 0;
}
	
