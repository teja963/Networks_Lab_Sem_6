#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<unistd.h>  
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>     
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>
#include<ctype.h>

#define IP_ADDR "127.0.0.1"

struct Login{
	char uname[20];
	char pwd[20];
};

struct Login cred[3];

void manage_mail(int sockID)
{
	char msg[256], mail[1024], user[20];
	int bytes = recv(sockID, user, sizeof(user), 0);
	
	strcat(user, "/mymailbox.mail");
	FILE *fp;
	fp = fopen(user, "r");
	char list[1024];
	bzero(list, sizeof(list));
	while(fgets(msg, sizeof(msg), fp)!=NULL)
	{
		if(strncmp(msg, "From:", 5)==0)
		{
			char *token1, *token2;
			const char s[2] = " ";
			token1 = strtok(msg, s);
			token2 = strtok(NULL, s);
			strcat(list, token2);
		}
		else if(strncmp(msg, "Received:", 9)==0)
		{
			strcat(list, msg);
			strcat(list, "\n");
		}
	}
	send(sockID, list, strlen(list), 0);
	fclose(fp);

	FILE *fp2;
	fp2 = fopen(user, "r");
	char list1[1024];
	bzero(list1, sizeof(list1));
	char sender[20];
	int attempts=1;
	while(attempts>=0)
	{
		while((bytes = recv(sockID, sender, sizeof(sender), 0))<0);
		int flag1=0;
		for(int i=0; i<3; i++)
		{
			sender[0] = toupper(sender[0]);
			if(strcmp(sender, cred[i].uname)==0)
			{
				flag1=1;
				break;
			}
		}
		sender[0] = tolower(sender[0]);
		if(flag1==0)
		{
			attempts -= 1;
			sprintf(msg, "INVALID MAIL-ID\n");
			send(sockID, msg, sizeof(msg), 0);
			continue;
		}
		else
			break;
	}
	if(attempts<0)
		return;

	int found=0;
	while(fgets(msg, sizeof(msg), fp2)!=NULL)
	{
		if(strncmp(msg, "From:", 5)==0)
		{
			char *token1, *token2, *token3;
			const char s[2] = "@";
			token1 = strtok(msg, s);
			const char s1[2] = " ";
			token2 = strtok(token1, s1);
			token3 = strtok(NULL, s1);
			if(strcmp(sender, token3)==0)
			{
				found=1;
			}
		}
	}
	if(found==0)
	{
		sprintf(msg, "NO MAILS TO SHOW\n");
		send(sockID, msg, sizeof(msg), 0);
		return;
	}
	FILE *fp3;
	fp3 = fopen(user, "r");
	 while(fgets(msg, sizeof(msg), fp2)!=NULL)
        {
                if(strncmp(msg, "From:", 5)==0)
                {
                        char *token1, *token2, *token3;
                        const char s[2] = "@";
                        token1 = strtok(msg, s);
                        const char s1[2] = " ";
                        token2 = strtok(token1, s1);
                        token3 = strtok(NULL, s1);
		}
	}	
}
			
void recv_mail(int socketID)
{
	char mail[1024], mail1[1024], mail2[1024];
	int bytes = recv(socketID, mail, sizeof(mail), 0);
	mail[bytes] = '\0';
	strcpy(mail1, mail);
	mail1[strlen(mail1)] = '\0';
	bzero(mail2, sizeof(mail2));
	const char s1[3] = "\n";
	char *token, *token1, *token2, *token3, *token4, *addr;
	
	token1 = strtok(mail,s1);
        token2 = strtok(NULL, s1);
	const char s2[2] = "@";
	token3 = strtok(token2, s2);
	const char s3[2] = " ";
	token4 = strtok(token3, s3);
	addr = strtok(NULL, s3);
	int i=0,flag=0;
	addr[0] = toupper(addr[0]);
	while(i<3)
	{
		if(strcmp(addr, cred[i++].uname)==0)
		{
			flag=1;
			break;
		}
	}
	addr[0] = tolower(addr[0]);
	if(flag==0)
	{
		printf("User does not exist, Mail deleted\n");
		fflush(stdout);
		return;
	}
	strcat(addr, "/mymailbox.mail"); 

	token = strtok(mail1, s1);
	while(token != NULL)
	{
		strcat(mail2, token);
		strcat(mail2, "\n");
		if(strncmp(token, "Subject:", 8)==0)
		{
			char msg[256];
			time_t rawtime;
		        struct tm * timeinfo;
  			time(&rawtime);
  			timeinfo = localtime(&rawtime);
			sprintf(msg, "Received: %d/%d/%d : %d hours : %d minutes\n",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min);
			strcat(mail2, msg);
		}
		token = strtok(NULL,s1);
	}
	FILE *Inbox;
	Inbox = fopen(addr, "a");
	fwrite(mail2, 1, strlen(mail2), Inbox);
	fclose(Inbox);
	printf("Mail received\n");
}

int main() 
{
	
	int PORT;
	printf("Enter port number: ");
	scanf("%d", &PORT);

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

	char buf[60];
	int i=0;
	FILE *fp;
	fp = fopen("logincred.txt", "r");
	while(1)
	{
		char buf[60];
		if(fgets(buf, 60, fp) == NULL)
			break;
		const char s[2] = ",";
		char *token1, *token2;

		token1 = strtok(buf,s);
		token2 = strtok(NULL, s);

		strcpy(cred[i].uname, token1);
		strcpy(cred[i].pwd, token2);
		i++;
	}

	//accept connections 
	while(1)
	{
		int socketID = accept(server_socket, NULL, NULL);
		
		char str[50];
		sprintf(str, "Connection is set up\n");
		send(socketID, str, sizeof(str), 0);

		while(1)
		{
			char uname[20];
			char pwd[20];
			char msg[50];

			int bytes = recv(socketID, uname, sizeof(uname), 0);
			uname[bytes] = '\0';

			int i=0;
			int flag=0;
			while(i<3)
			{
				if(strcmp(uname, cred[i].uname)==0)
				{	
					flag=1;
					break;
				}
				i++;
			}

			if(flag==0)
			{
				//bzero(msg, sizeof(msg));
				sprintf(msg, "Incorrect Username\n");
				send(socketID, msg, sizeof(msg), 0);
				continue;
			}
			
			//bzero(msg, sizeof(msg));
			strcpy(msg, "Correct Username; Enter password: ");
			fflush(stdout);
			send(socketID, msg, sizeof(msg), 0);
			bytes = recv(socketID, pwd, sizeof(pwd), 0);
			pwd[bytes] = '\0';
			if(strcmp(pwd, cred[i].pwd)==-10)
			{
				bzero(msg, sizeof(msg));
				sprintf(msg, "Authentication successful\n");
				send(socketID, msg, sizeof(msg), 0);
				break;
			}
			else
			{
				bzero(msg, sizeof(msg));
				sprintf(msg, "Incorrect Password\n");
				send(socketID, msg, sizeof(msg), 0);
				continue;
			}
		}
		char command[20];
		while(1)
		{
			recv(socketID, command, sizeof(command), 0);
			if(strcmp(command, "Quit")==0)
				break;
			manage_mail(socketID);	
		}
		close(socketID);
	}
}
