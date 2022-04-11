#include "inc.c"
#define BUFF_MAX 500
#define SIZE 500
#define PORT_NUMBER 8080
#define SA struct sockaddr
#define MAX_LINE 500
#define BUFFSIZE 500

typedef struct Acc{
    char username[50];
    char password[50];
} Acc;

ssize_t total=0;

void get_time_string(char *time_string){
    time_t current_time;
    struct tm *timeinfo;

    current_time = time(NULL);
    timeinfo = localtime(&current_time);
    strftime(time_string, 100, "%a, %d %b %Y %I:%M:%S Localtime", timeinfo);
}
// Driver function 
int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
	char username[50], password[50];
	int n; 
	// socket create and verification ; IpV4 PROTOCOL; TCP; IP=0
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	memset(&servaddr,0, sizeof(servaddr));
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT_NUMBER);

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Socket bind failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed\n"); 
		exit(0); 
	} 
	else
		printf("Server listening\n"); 
	len = sizeof(cli); 

	

    char *filename = "send";

	FILE *cred_file = fopen("logincred.txt", "r");

    char fusername[50], fpassword[50];

    int total = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        total++;

    Acc *creds = calloc(total, sizeof(Acc));

    fseek(cred_file, 0, SEEK_SET);
    int i = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
    {
        strcpy(creds[i].username, fusername);
        strcpy(creds[i].password, fpassword);
        i++;
    }
	char *invalidPwd = "Invalid Password";
    char *invalidUser = "Invalid Username";
    char *success = "Success";

	// Accept the data packet from client and verification 
		connfd = accept(sockfd, (SA*)&cli, &len); 
		if (connfd < 0) { 
			printf("Server acccept failed\n"); 
			exit(0); 
		} 
		else
			printf("Server acccept the client\n"); 
		
		bzero(username, 0);
		bzero(password, 0);
		int flag = 0;
		if(recv(connfd, &username, 50, 0) < 0)
			perror("Error in recv()");
		if(recv(connfd, &password, 50, 0) < 0)
			perror("Error in recv()");
		printf("%s\n", username);
		printf("%s\n", password);

		for(int i = 0; i < total; i++)
		{
			if (strcmp(creds[i].username, username) == 0)
			{
				if (strcmp(creds[i].password, password) != 0)
				{
					send(connfd, invalidPwd, sizeof(invalidPwd),0 );
					close(connfd);
				} 
				else 
				{
					flag = 1;
					send(connfd, success, sizeof(success),0 );
					break;

				}
        	}
		}
		if(flag == 0)
		{
			send(connfd, invalidUser, strlen(invalidUser), 0);
			close(connfd);
		}
		printf("[+] Client %s authenticated successfully.\n", username);

	while(1)
	{
		bzero(buff1,0);
		bzero(buff2,0);

		strcpy(buff2, "gibberish\n");
		
		recv(connfd, &buff1, sizeof(buff1), 0);
	
		if(strcmp(buff1, "EXIT") == 0)
		{
			printf("Client- %s left. ", username);
			close(connfd);
			break;
		}
		else 
		{
			printf("[+] Received new mail\n");

			char from[50], to[50], subject[50], body[1024];

			get_field(buff1, "From", from);
			get_field(buff1, "To", to);
			get_field(buff1, "Subject", subject);
			get_field(buff1, "Body", body);

			
			// Verify Emails
			if (!verify_email(from)){
				char *invalidFrom = "Invalid";
				printf("[X] Invalid Sender Email\n");
				send(connfd, invalidFrom, sizeof(invalidFrom), 0);
				continue;
			}

			if (!verify_email(to)){
				char *invalidTo = "Invalid";
				printf("[X] Invalid Recepient Email\n");
				send(connfd, invalidTo, strlen(invalidTo), 0);
				continue;
			}

			char send_username[50] = {0};
			// Check Recepient
			int p = has_char(to, '@');
			int rec_flag = 0;
			strncpy(send_username, to, p);
			send_username[p] = 0;

			for(int i = 0; i < total; i++){
				if (strcmp(creds[i].username, send_username) == 0)
					rec_flag = 1;
			}

			printf("this is being sent %s %s\n",send_username, username);
			if (!rec_flag){
				printf("[X] Recepient %s not found\n", to);
				char emailError[50] = "Invalid Email";
				send(connfd, emailError, sizeof(emailError), 0);
				continue;
			}

			
			char from_username[50] = {0};
			int p1 = has_char(from, '@');
			strncpy(from_username, from, p1);
			from_username[p1] = 0;
			
			if(strcmp(from_username, username) != 0)
			{
				printf("[X] Sender is not valid - %s\n", from);
				char emailError[50] = "Invalid Email";
				send(connfd, emailError, sizeof(emailError), 0);
				continue;
			}

			char mailbox_fname[100] = {0};
			sprintf(mailbox_fname, "%s/mymailbox.mail", send_username);
			FILE *mailbox = fopen(mailbox_fname, "a");

			char time_str[50];
			get_time_string(time_str);

			fprintf(mailbox, "From: %s\n", from);
			fprintf(mailbox, "To: %s\n", to);
			fprintf(mailbox, "Subject: %s\n", subject);
			fprintf(mailbox, "Received: %s\n", time_str);
			fprintf(mailbox, "%s", body);

			fclose(mailbox);
			
			char succ[50] = "EMAIL SENT";
			printf("[+] %s has sent mail to %s.\n", from, to);
			send(connfd, succ, sizeof(succ), 0);

			bzero(buff1, 0);

		}
		memset(&buff1, 0, strlen(buff1)+1);
		memset(&buff2, 0, strlen(buff2)+1);
	}    
	close(sockfd);
} 