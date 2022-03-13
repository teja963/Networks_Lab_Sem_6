#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 8080

int check();
int check_username();
int check_password();

int main() {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		perror("Socket");
		exit(0);
	}

	printf("SOCKET CREATED................\n");

	struct sockaddr_in server_addr, client_addr;
	int server_addr_len = sizeof(server_addr), client_addr_len = sizeof(client_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr)) <= 0) {
		perror("Address Conversion");
		exit(0);
	}

	if(bind(sock_fd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
		perror("Bind");
		exit(0);
	}

	printf("BINDED SUCCESSFULLY...............\n");

	if(listen(sock_fd, 5) == -1) {
		perror("Listen");
		exit(0);
	}

	printf("LISTENING.................\n");

	int connection_fd = accept(sock_fd, (struct sockaddr*) &client_addr, &client_addr_len);
	if(connection_fd == -1) {
		perror("Accept");
		exit(0);
	}

	printf("CONNECTED TO CLIENT.................\n");

	int temp;
	char buffer[501], buffer_1[1034];
	char username[1024];
	int state = 0;

	for(;;) {
		temp = recv(connection_fd, buffer, sizeof(buffer), 0);
		
		if(strncmp("USERN", buffer, 5) == 0 && state == 0) {
			
			FILE *fp = fopen("logincred.txt", "r");
			char string[1024];

			while(fscanf(fp, "%s", string) != -1) {
				if(check_username(buffer + 6, string)) {
					state = 1;
					printf("%s\n", buffer + 6);
					strcpy(username, buffer + 6);

					temp = send(connection_fd, "300", 4, 0);
					if(!check(connection_fd, "Send", temp))
						break;

					temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
					check(connection_fd, "Receive", temp);
				}
			}

			if(state == 0) {
				temp = send(connection_fd, "301", 4, 0);
				if(!check(connection_fd, "Send", temp))
					break;

				temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
				check(connection_fd, "Receive", temp);
			}

			fclose(fp);  
		}
		else if(strncmp("PASSWD", buffer, 6) == 0 && state == 1) {

			FILE *fp = fopen("logincred.txt", "r");
			char string[1024];

			while(fscanf(fp, "%s", string) != -1) {
				if(check_password(buffer + 7, string)) {
					state = 2;
					
					temp = send(connection_fd, "305", 4, 0);
					if(!check(connection_fd, "Send", temp))
						break;

					temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
					check(connection_fd, "Receive", temp);

					sprintf(buffer_1, "Welcome %s", username);
					temp = send(connection_fd, buffer_1, strlen(buffer_1) + 1, 0);
					if(!check(connection_fd, "Send", temp))
						break;

					temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
					check(connection_fd, "Receive", temp);	
				}
			}

			if(state == 1) {
				temp = send(connection_fd, "310", 4, 0);
				if(!check(connection_fd, "Send", temp))
					break;

				temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
				check(connection_fd, "Receive", temp);
			} 

			fclose(fp);
		}

		else if(strncmp("QUIT", buffer, 4) == 0) {
			printf("CLIENT CLOSED THE CONNECTION\n");
			send(connection_fd, "QUIT", 4, 0);
			close(connection_fd);
			break;
		}

		else if(strncmp("CreateFile", buffer, 10) == 0 && state == 2) {
			char *str = buffer + 11;
			printf("%s %ld\n", str, strlen(str));
			FILE *fp = fopen(str, "r");

			if(fp == NULL)
				printf("FAILED TO CREATE FILE\n");
			else
				fclose(fp);
		}

		else if(strncmp("GetFile", buffer, 7) == 0) {
			char *str = buffer + 8;
			FILE *fp = fopen(str, "r");

			temp = send(connection_fd, "GETFILE", 4, 0);
			check(connection_fd, "Send", temp);

			temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
			check(connection_fd, "Receive", temp);

			temp = send(connection_fd, str, strlen(str) + 1, 0);
			check(connection_fd, "Send", temp);

			temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
			check(connection_fd, "Receive", temp);

			if(fp == NULL)
				printf("FAILED TO OPEN FILE\n");
			else {
				printf("OPENED\n");
				while(fgetc(fp) != EOF) {
					send(connection_fd, str, 1, 0);
					check(connection_fd, "Send", temp);

					temp = recv(connection_fd, buffer_1, sizeof(buffer_1), 0);
					check(connection_fd, "Receive", temp);
				}
				temp = send(connection_fd, '\0', 1, 0);
				check(connection_fd, "Send", temp);

				fclose(fp);
			}
			
		}

		else {
			temp = send(connection_fd, "505", 4, 0);
			if(!check(connection_fd, "Send", temp))
				exit(0);
		}
	}

	close(connection_fd);
	close(sock_fd);
}

int check_username(char *str_1, char *str_2) {

	for(int i = 0; ; i++) {
		if (str_1[i] == '\0' && str_2[i] != ',') 
			return 0;

		else if(str_1[i] != '\0' && str_2[i] == ',')
			return 0;

		else if(str_1[i] == '\0' && str_2[i] == ',')
			return 1;

		else if(str_1[i] != str_2[i])
			return 0;
	}

}

int check_password(char *str_1, char *str_2) {
	int i, j;

	for(i = 0; str_2[i] != ','; i++);
	i++;

	for(j = 0; ;j++, i++) {
		if (str_1[j] == '\0' && str_2[i] != '\0') 
			return 0;
		
		else if(str_1[j] != '\0' && str_2[i] == '\0') 
			return 0;

		else if(str_1[j] != str_2[i]) 
			return 0;
		
		else if(str_1[j] == '\0' && str_2[i] == '\0') 
			return 1;
		
	}
}

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
