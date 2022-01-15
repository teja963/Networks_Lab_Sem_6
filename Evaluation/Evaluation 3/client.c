#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 8080

int check();

int main(int argc, char *argv[]) {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		perror("Socket");
		exit(0);
	}

	struct sockaddr_in serv_addr, client_addr;
	int serv_addr_len = sizeof(serv_addr), client_addr_len = sizeof(client_addr);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr)) <= 0) {
		perror("Address Conversion");
		exit(0);
	}

	//***********************************************************************************************
	//								Custom Address and Port for client
	//								argv[1] = Client IP Address
	//								argc[2] = Client Port
	//***********************************************************************************************
	// if(argc == 3) {
	// 	int port;
	// 	sscanf(argv[2], "%d", &port);
	// 	printf("%s\n%d\n", argv[1], port);
	// 	client_addr.sin_family = AF_INET;
	// 	client_addr.sin_port = htons(port);
	// 	if(inet_pton(AF_INET, argv[1], &(client_addr.sin_addr)) <= 0) {
	// 		perror("Address Conversion");
	// 		exit(0);
	// 	}
	// 	if(bind(sock_fd, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
	// 		perror("Bind");
	// 		exit(0);
	// 	}
	// }
	//***********************************************************************************************


	int temp, state;

	char buffer[1024];


	for(;;) {
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
			perror("Fgets");
			exit(0);
		}
		buffer[strlen(buffer) - 1] = '\0';

		if(strncmp(buffer, "START", 5) == 0)
			break;
		else if(strncmp(buffer, "QUIT", 4) == 0)
			exit(0);
	}

	printf("CONNECTED..................\n");
	temp = connect(sock_fd, (struct sockaddr *)&serv_addr, serv_addr_len);
	if(temp == -1) {
		perror("Connect");
		exit(0);
	}

	int pid = fork();
	if(pid == -1) {
		perror("Fork");
		close(sock_fd);
		exit(0);
	}
	else if(pid == 0) {
		for(;;) {

			if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
				perror("Fgets");
				close(sock_fd);
				exit(0);
			}
			buffer[strlen(buffer) - 1] = '\0';

			// if(strcmp("QUIT", buffer, 4) == 0) {
			// 	close(sock_fd);
			// 	exit(0);
			// }

			temp = send(sock_fd, buffer, strlen(buffer) + 1, 0);
			if(!check(sock_fd, "Send", temp)){
				break;
				exit(0);
				close(sock_fd);
			}
			else {
				if(temp != (strlen(buffer) + 1)) 
					printf("SOME PART OF THE MESSAGE HAS SENT TO THE SERVER\n");

				if(strcmp(buffer, "QUIT") == 0) {
					close(sock_fd);
					exit(0);
				}
			}				
		}
	}
	else {
		for(;;) {
			temp = recv(sock_fd, buffer, sizeof(buffer), 0);
			if(!check(sock_fd, "Receive", temp)) 
				break;
			
			if(strncmp(buffer, "QUIT", 4) == 0)
				exit(0);
			
			else if(strncmp(buffer, "GETFILE", 6) == 0) {
				temp = send(sock_fd, "OK", 2, 0);
				check(sock_fd, "Receive", temp);

				temp = recv(sock_fd, buffer, sizeof(buffer), 0);
				check(sock_fd, "Receive", temp);

				printf("FILE: %s\n", buffer);

				temp = send(sock_fd, "OK", 2, 0);
				check(sock_fd, "Receive", temp);

				FILE *fp = fopen(buffer, "w");

				while(1){
					temp = recv(sock_fd, buffer, sizeof(buffer), 0);
					if(buffer[0] == '\0')
						break;
					fprintf(fp, "%s", buffer);
				}

			}
			else 
				printf("%s\n", buffer);

			temp = send(sock_fd, "OK", 2, 0);
			check(sock_fd, "Send", temp);
		}
		
	}
}


// *******************Check*********************************
// Returns 0 if there is an error while sneding or receiving
// Returns 1 if there is no error
// *********************************************************
int check(int sock_fd, char *str, int return_val) {
	
	if(return_val == 0) {
		printf("CONNECTION CLOSED\n");
		close(sock_fd);
		return 0;
	}
	
	else if(return_val == -1) {
		perror(str);
		close(sock_fd);
		return 0;
	}

	return 1;
}