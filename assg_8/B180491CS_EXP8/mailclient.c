#include "inc.c"
#define BUFF_MAX 500
#define SIZE 500
#define PORT 8024
#define SA struct sockaddr_in
#define MAX_LINE 500
#define BUFFSIZE 500
#define LINE_SIZE 80
#define MAX_LINES 50
#define MAX_BODY_SIZE LINE_SIZE *MAX_LINES + 1
#define BUFFER_SIZE 1024

void hr(){
    for (int i = 0; i < 80; i++)
        printf("-");
    printf("\n");
}

void main(int argc, char *argv[])
{
    if(argc<2)//If IP address is not provided
    {
        printf("Server IP address and port number not provided.\n");
        exit(1);
    }
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	char buff1[BUFF_MAX], buff2[BUFF_MAX]; 
	int n, len; 
	char username[50], password[50];
	char check[50];
	// socket create and vearification 
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
    servaddr.sin_port = htons(atoi(argv[1]));
	
	/* Connect to server the server */
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server.\n"); 
	bzero(buff1, sizeof(buff1));
	bzero(buff2, sizeof(buff2));
	int nb;
	printf("Enter the username: ");
	scanf("%s", username);
	printf("Enter the password: ");
	scanf("%s", password);

	send(sockfd, username, sizeof(username), 0);
	send (sockfd, password, sizeof(password), 0);
	
	check[0] = '\0';
	char ch [50];
	int l = recv(sockfd, &ch, sizeof(ch), 0);
	
	if (strcmp(ch, "Success") != 0)
    {
        printf("Error authenticating user\n");
        close(sockfd);
        exit(0);
    }
	//exit(0);
	printf("You are authenticated!\n");
	int choice = 0;
	printf("\nWelcome to SMTP client\n");
    printf("1. Send Mail\n");
    printf("2. Quit\n");
   
    
	
	while (1)
    {
        hr();
        printf("Select option [1 or 2]: ");
        scanf("%d%*c", &choice);
        printf("\n");

        if (choice == 2)
        {
            printf("Bye\n");
            char scheck[] = "EXIT";
            send(sockfd, scheck, sizeof(scheck), 0);
            break;
        }

        if (choice < 1 || choice > 2)
        {
            printf("Invalid choice %d\nPlease try again\n", choice);
            continue;
        }

        if (choice == 1)
        {
            printf("Option selected: Send new email\n");
            printf("To end the body, last line should be just a period (\".\")\n");
            char from[50], to[50], subject[50], body[MAX_BODY_SIZE];
            char buffer[LINE_SIZE] = {0};
            int count = 0;
            check[0] = '\0';
            printf("From: ");
            scanf("%s%*c", from);
            printf("To: ");
            scanf("%s%*c", to);
            printf("Subject: ");
            scanf("%[^\n]%*c", subject);

            if (!verify_email(to) || !verify_email(from)){
                printf("Incorrect format\n");
                continue;
            }
            
            while (strcmp(buffer, ".") != 0 && count < MAX_BODY_SIZE){
                scanf("%[^\n]%*c", buffer);
                count += sprintf(body + count, "%s\n", buffer);
            }
			printf("%s", body);
            char data[BUFFER_SIZE];

            count = 0;
            count += sprintf(data + count, "From: %s\n", from);
            count += sprintf(data + count, "To: %s\n", to);
            count += sprintf(data + count, "Subject: %s\n", subject);
            count += sprintf(data + count, "%s\n", body);

            send(sockfd, data,count,0);
            check[0] = '\0';
            recv(sockfd, &check, sizeof(check), 0);

            if (strcmp("EMAIL SENT", check) == 0){
                printf("Email sent succesffully!\n");
                continue;
            }
            else{
                printf("Error sending email!\n");
                printf("Error: %s\n", check);
            }

            check[0] = '\0';
        }
    }

    close(sockfd);
    exit(1);
} 
