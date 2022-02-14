#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<ctype.h>

#define IP_ADDR "127.0.0.1"
char user[20];

void manage_mail(int sockID)
{
	send(sockID, user, strlen(user), 0);
	char list[1024];
	int bytes = recv(sockID, list, sizeof(list), 0);
	printf("%s", list);

	int attempts=1;
	while(1)
	{
		char msg[256], sender[20];
		printf("Enter sender Mail ID\n");
		scanf("%s", sender);
		send(sockID, sender, sizeof(sender), 0);
		bytes = recv(sockID, msg, sizeof(msg), 0);
		if(strcmp(msg, "INVALID MAIL-ID\n")==0)
		{
			printf("%s", msg);
			if(attempts==1)
			{
				attempts = 0;
				continue;
			}
			else
				break;
		}
		else if(strcmp(msg, "NO MAILS TO SHOW\n")==0)
		{
			printf("%s", msg);
			break;
		}
		else
		{
			char list1[1024];
			bytes = recv(sockID, list1, sizeof(list1), 0);
			printf("%s", list1);
			char c;
			while((c = getchar()) != 113)
				printf("%s", list1);
			break;
		}
	}
}
























int check_line_format(char *line)
{
	const char s[2] = " ";
	char *token;
	token = strtok(line, s);
	token = strtok(NULL, s);
	int n = strlen(token), l=0, r=0, flag=0;
    	for(int i=0;i<n;i++)
	{
        	if(token[i]=='@')
		{
            		flag = 1;
            		continue;
        	}
        	if(flag==0)
           		l++;
        	else
            		r++;
        	if (r>= 1)
            		break;
    	}
    	return l>0 && r>0;
}

int check_mail_format(char email[1024])
{
	char mail[1024];
	strcpy(mail, email);
	int flag=0;
	const char s[3] = "\n";
	char *token;
	char *mail1 = mail;
	token = strtok_r(mail1, s, &mail1);
	while(token != NULL)
	{
		char token1[256];
		strcpy(token1, token);
		switch(flag)
		{
			case 0: if((strncmp(token1, "From:", 5)==0) && check_line_format(token1))
					flag=1;
				else
					return 0;
				break;

			case 1: if((strncmp(token1, "To:", 3)==0) && check_line_format(token1))
					flag=2;
				else
					return 0;
				break;

			case 2: if(strncmp(token1, "Subject:", 8)==0)
					return 1;
				break;
		}
		token = strtok_r(NULL, s, &mail1);
	}
}

int check_sender(char msg[256])
{
	char line[256];
	strcpy(line, msg);

	const char s1[2] = "@";
	char *token1,*token2,*token3;
	token1 = strtok(line, s1);
	const char s2[2] = " ";
	token2 = strtok(token1, s2);
	token3 = strtok(NULL, s2);
	if(strcmp(user, token3)!=0)
		return 0;
	else
		return 1;
}

void send_mail(int sockID)
{
	while(1)
	{
		char mail[1024];
		char msg[256];
		bzero(mail, sizeof(mail));
		bzero(msg, sizeof(msg));
		printf("Enter mail body\n");
		while(1)
		{
			scanf("%[^\n]%*c",msg);
			strcat(mail, msg);
			strcat(mail, "\n");
			if(strncmp(msg, "From:", 5)==0)
			{
				if(check_sender(msg)!=1)
				{
					printf("Username and From address mismatch\n");
					break;
				}
			}

			if(strcmp(msg, ".")==0)
				break;
		}
		if(!check_mail_format(mail))
		{
			printf("Incorrect format\n");
			continue;
		}
		else
		{
			send(sockID, mail, sizeof(mail), 0);
			break;
		}
	}
	printf("Mail sent successfully\n");
}		

int open_connection(int PORT)
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
        char str[50];
        int bytes = recv(client_socket, str, sizeof(str), 0);
        str[bytes] = '\0';
        printf("%s", str);
        int sockID = client_socket;
	return sockID;
}

int main()
{

	/*
	int PORT;
	printf("Enter port number: ");
	scanf("%d", &PORT);

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
	char str[50];
	int bytes = recv(client_socket, str, sizeof(str), 0);
	str[bytes] = '\0';
	printf("%s", str);
	int sockID = client_socket;
	*/
	int sockID = open_connection(7002); // pop3 port

	while(1)
	{
		char uname[20];
		char pwd[20];
		char msg[50];

		printf("Enter Username: ");
		scanf("%s", uname);
		send(sockID, uname, sizeof(uname), 0);
		int bytes;
		while((bytes=recv(sockID, msg, sizeof(msg), 0))<0);
		msg[bytes] = '\0';
		printf("%s", msg);
		fflush(stdout);
		
		if(strcmp(msg, "Incorrect Username\n")==0)
		{
			printf("Retry\n");
			continue;
		}
		strcpy(user, uname);
		user[0] = tolower(user[0]);
		scanf("%s", pwd);
		send(sockID, pwd, sizeof(pwd), 0);
		bytes = recv(sockID, msg, sizeof(msg), 0);
		msg[bytes] = '\0';
		printf("%s",msg);

		if(strcmp(msg, "Incorrect Password\n")==0)
		{
			printf("Retry\n");
			continue;
		}
		else
			break;
	}
	//close(sockID);
	//printf("Connection to POP3 server is closed\n");

	char command[20];
	while(1)
	{
		printf("1.Send Mail\n2.Manage Mail\n3.Quit\n");
		printf("Enter command\n");
		scanf(" %[^\n]%*c", command);
		int flag=0;
		if(strcmp(command, "Quit")==0)
		{
			send(sockID, command, sizeof(command), 0);
			break;
		}
		else if(strcmp(command, "Send Mail")==0)
		{
			//int sockID = open_connection(7001); //smtp server
			send(sockID, command, sizeof(command), 0);
			send_mail(sockID);
			//close(sockID); //connection closed
		}
		else if(strcmp(command, "Manage Mail")==0)
		{
			//int sockID = open_connection(7002); //pop3 server
			send(sockID, command, sizeof(command), 0);
			manage_mail(sockID);
			//close(sockID);
		}
		else
			printf("Invalid command\n");
	}
	close(sockID);
}


