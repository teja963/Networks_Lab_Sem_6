#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#include<sys/select.h>

#define PORT 8182

int check();

int main() {
	int server_fd;
	int max_fd;

	fd_set read_fds;
	fd_set master;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(server_fd == -1) {
		perror("Socket");
		exit(0);
	}

	max_fd = server_fd;

	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int serv_addr_len = sizeof(serv_addr);
	int client_addr_len = sizeof(client_addr);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, "127.0.0.2", &(serv_addr.sin_addr)) <= 0) {
		perror("Address Conversion");
		exit(0);
	}

	if(bind(server_fd, (struct sockaddr *)&serv_addr, serv_addr_len) == -1) {
		perror("Bind");
		exit(0);
	}

	if(listen(server_fd, 3) == -1) {
		perror("Listen");
		exit(0);
	}

	printf("I am listening on port 8182\n");

	FD_ZERO(&read_fds);
	FD_ZERO(&master);

	int temp, i, j, sum, new_connection;
	char buffer_1[1024], buffer_2[1024];
	char client[10][256];
	char password[10] = "234513";
	char server[10] = "Richard";

	FD_SET(server_fd, &master);

	for(;;) {
		read_fds = master;


		if(select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Select");
			exit(0);
		}

		for(i = 0; i < max_fd + 1; i++) {
			if(FD_ISSET(i, &read_fds)) {


				if(i == server_fd) {
					new_connection = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

					if(new_connection == -1) 
						perror("Accept");
					else {
						FD_SET(new_connection, &master);


						strcpy(buffer_1, "Enter Your Passwd:");
						temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
						if(!check(new_connection, "Send", temp, &master))
							continue;

						temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
						if(!check(new_connection, "Receive", temp, &master))
							continue;

						temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
						if(check(new_connection, "Receive", temp, &master)) {
							printf("%s\n", buffer_1);
							if(strcmp(password, buffer_1) != 0) {
								printf("wrong\n");
								temp = send(new_connection, "Wrong Password", strlen("Wrong Password"), 0);
								if(check(new_connection, "Send", temp, &master)) {
									close(new_connection);
									FD_CLR(new_connection, &master);
									continue;
								}
							}	
							else {
								temp = send(new_connection, "OK", strlen("OK") + 1, 0);
								if(!check(new_connection, "Send", temp))
									exit(0);
														}
						}

						temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
						if(check(new_connection, "Receive", temp, &master))
							strcpy(client[new_connection], buffer_1);

						strcpy(buffer_1, "Welcome to Project Cobra");
						temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
						if(!check(new_connection, "Send", temp, &master))
							continue;

						temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
						if(!check(new_connection, "Receive", temp, &master))
							continue;

						sum = 0;
						for(j = 0; j < max_fd + 1; j++) {
							if((j == server_fd) || (j == new_connection))
								continue;

							if(FD_ISSET(j, &master))
								sum++;
						} 
						
						if(sum == 0) {
							strcpy(buffer_1, "No Other Co-PIs connected\nPl. wait.");
							temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
							if(!check(new_connection, "Send", temp, &master))
								continue;

							temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
							if(!check(new_connection, "Receive", temp, &master))
								continue;
						}
						else {
							sum = 1;

							strcpy(buffer_1, "The Co-PIs connected are");
							temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
							if(!check(new_connection, "Send", temp, &master))
								continue;

							temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
							if(!check(new_connection, "Receive", temp, &master))
								continue;

							for(j = 0; j < max_fd + 1; j++) {
								if((j == server_fd) || (j == new_connection))
									continue;

								if(FD_ISSET(j, &master)) {

									sprintf(buffer_1, "%d. %s", sum, client[j]);
									sum++;
									
									temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
									if(!check(new_connection, "Send", temp, &master))
										continue;

									temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
									if(!check(new_connection, "Receive", temp, &master))
										continue;
								}
							}
						}


						strcpy(buffer_1, "end");
						temp = send(new_connection, buffer_1, strlen(buffer_1) + 1, 0);
						if(!check(new_connection, "Send", temp, &master))
							continue;

						temp = recv(new_connection, buffer_1, sizeof(buffer_1), 0);
						if(!check(new_connection, "Receive", temp, &master))
							continue;

						if(FD_ISSET(new_connection, &master) && max_fd < new_connection)
							max_fd = new_connection;

						printf("%s connected\n", client[new_connection]);

						sprintf(buffer_1, "%s connected", client[new_connection]);
						for(j = 0; j < max_fd + 1; j++) {
							if(j == server_fd || j == new_connection)
								continue;
							if(FD_ISSET(j, &master)) {
								temp = send(j, buffer_1, strlen(buffer_1) + 1, 0);
								if(!check(new_connection, "Send", temp, &master))
									continue;
							}
						}
					}
				}

				else {
					temp = recv(i, buffer_1, sizeof(buffer_1), 0);

					if(check(i, "Receive", temp, &master)){
						for(j = 0; j < max_fd + 1; j++) {
							if(j == server_fd || j == i)
								continue;

							if(FD_ISSET(j, &master)) {
								sprintf(buffer_2, "[%s]: %s", client[i], buffer_1);
								temp = send(j, buffer_2, strlen(buffer_2) + 1, 0);
								check(j, "Send", temp, &master);
							}
						}

						if(strcmp(buffer_1, "Bye") == 0) {
							printf("%s left the discussion\n", client[i]);
							sprintf(buffer_1, "%s left the discussion", client[i]);
							for(j = 0; j < max_fd + 1; j++) {
								if(j == server_fd || j == i)
									continue;
								if(FD_ISSET(j, &master)) {
									temp = send(j, buffer_1, strlen(buffer_1) + 1, 0);
									if(!check(new_connection, "Send", temp, &master))
										continue;
								}
							}

							close(i);
							FD_CLR(i, &master);
						}
					}	
				}	
			}
		}
	}
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
}

// *******************Check*********************************
// Returns 0 if there is an error while sneding or receiving
// Returns 1 if there is no error
// *********************************************************
int check(int sock_fd, char *str, int return_val, fd_set *master) {
	
	if(return_val == 0) {
		printf("CONNECTION CLOSED : %d\n", sock_fd);
		FD_CLR(sock_fd, master);
		close(sock_fd);
		return 0;
	}
	
	else if(return_val == -1) {
		perror(str);
		FD_CLR(sock_fd, master);
		close(sock_fd);
		return 0;
	}
	
	return 1;
}