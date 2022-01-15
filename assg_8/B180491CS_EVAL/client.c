#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "packet.c"
#include "fields.c"

#define PORT 5000
#define POP_PORT 6000
#define LINE_SIZE 81
#define MAX_LINES 51
#define MAX_BODY_SIZE LINE_SIZE *MAX_LINES + 1
#define BUFFER_SIZE 1024

void hr()
{
    for (int i = 0; i < 80; i++)
        printf("-");
    printf("\n");
}

void show_usage()
{
    printf("Usage: ");
    printf("./client.o PORT\n");
    printf("PORT is the port number to use to connect to the SMTP server\n");
}

int main(int argc, char *argv[])
{
    int port = PORT;
    int pop_port = POP_PORT;
    char *address = "127.0.0.1";
    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            show_usage();
            return 0;
        }
        if (sscanf(argv[1], "%d", &port) != 1)
        {
            printf("Invalid PORT number\n");
            show_usage();
            return -1;
        }
    }
    else if (argc == 1)
    {
        printf("PORT not specified\n");
        printf("Using the default port as %d\n", port);
    }
    else
    {
        printf("Invalid arguments\n");
        show_usage();
        return -1;
    }

    int sock_fd = 0, pop_fd = 0, recv_len;
    struct sockaddr_in server_address, pop_address;
    char username[50], password[50];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }

    if ((pop_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    pop_address.sin_family = AF_INET;
    pop_address.sin_port = htons(pop_port);

    if (inet_pton(AF_INET, address, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }

    if (inet_pton(AF_INET, address, &pop_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }
    printf("Connecting...\n");

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("SMTP Connection failed!\n");
        return -1;
    }

    if (connect(pop_fd, (struct sockaddr *)&pop_address, sizeof(pop_address)) < 0)
    {
        printf("Pop Connection failed!\n");
        return -1;
    }
    printf("Successfully connected to %s:%d\n", address, port);

    printf("Please enter username and password to continue...\n");
    printf("Username: ");
    scanf("%s%*c", username);

    printf("Password: ");
    scanf("%s%*c", password);

    int length;
    char *response;

    send_packet(sock_fd, username, strlen(username));
    send_packet(sock_fd, password, strlen(password));

    send_packet(pop_fd, username, strlen(username));
    send_packet(pop_fd, password, strlen(password));

    length = recv_packet(sock_fd, &response);
    length = recv_packet(pop_fd, &response);

    if (strcmp(response, "AUTHENTICATED") != 0)
    {
        printf("Error authenticating user\n");
        printf("%s\n", response);

        close(sock_fd);

        return -1;
    }

    free(response);

    printf("Authenticated!\n");
    int choice = 0;

    while (1)
    {
        hr();
        printf("\nWelcome to SMTP client\n");
        printf("1. Send Mail\n");
        printf("2. Manage Mail\n");
        printf("3. Quit\n");
        printf("Select one option [1-3]: ");
        scanf("%d%*c", &choice);
        printf("\n");

        if (choice == 3)
        {
            break;
        }

        if (choice < 1 || choice > 3)
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

            printf("From: ");
            scanf("%s%*c", from);
            printf("To: ");
            scanf("%s%*c", to);
            printf("Subject: ");
            scanf("%[^\n]%*c", subject);

            if (!verify_email(to))
            {
                printf("Invalid recepient email!\nEmail should be of the format X@Y\n");
                continue;
            }
            if (!verify_email(from))
            {
                printf("Invalid sender email!\nEmail should be of the format X@Y\n");
                continue;
            }
            printf("Message body:\n");
            while (strcmp(buffer, ".") != 0 && count < MAX_BODY_SIZE)
            {
                scanf("%[^\n]%*c", buffer);
                count += sprintf(body + count, "%s\n", buffer);
            }

            char data[BUFFER_SIZE];

            count = 0;
            count += sprintf(data + count, "From: %s\n", from);
            count += sprintf(data + count, "To: %s\n", to);
            count += sprintf(data + count, "Subject: %s\n", subject);
            count += sprintf(data + count, "%s\n", body);

            send_packet(sock_fd, data, count);

            recv_packet(sock_fd, &response);

            if (strcmp("EMAIL SENT", response) == 0)
            {
                printf("Email sent succesffully!\n");
                continue;
            }
            else
            {
                printf("Error sending email!\n");
                printf("Error: %s\n", response);
            }

            free(response);
        }

        if (choice == 2)
        {
            printf("Manage mail options selected\n");
            char *request = "GET_ALL";
            send_packet(pop_fd, request, strlen(request));
            int len = recv_packet(pop_fd, &response);
            char filter[100];

            if (len == -1)
            {
                printf("Data corrupted exiting...\n");
                break;
            }

            printf("Mails recieved:\n%s\n", response);

            free(response);

            int flag = 0;

            printf("Filter (sender email): ");
            scanf("%s%*c", filter);

            if (verify_email(filter))
            {
                flag = 1;
            }
            if (!flag)
            {
                printf("INVALID MAIL_ID\n");
                printf("Filter (sender email): ");
                scanf("%s%*c", filter);
                if (verify_email(filter))
                {
                    flag = 1;
                }
            }

            if (flag)
            {
                send_packet(pop_fd, "GET_FILTER", strlen("GET_FILTER"));
                send_packet(pop_fd, filter, strlen(filter));
                recv_packet(pop_fd, &response);
                printf("Received mails (%s):\n%s\n", filter, response);
            }
        }
    }

    printf("Bye\n");
    response = "EXIT";
    send_packet(sock_fd, response, strlen(response));
    send_packet(pop_fd, response, strlen(response));
    close(sock_fd);
    return 0;
}
