/*
 * An SMTP server application
 * Author   -   Anish Sharma
 * Date     -   18-03-2021
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "packet.c"
#include "fields.c"

#define PORT 5000

typedef struct Cred
{
    char username[50];
    char password[50];
} Cred;

typedef struct Client
{
    char *username;
    char *password;
    int sock_fd;

    Cred *creds;
    int no_of_users;

    struct Client *head;
    struct Client *next;
} Client;

Client *new_client(int sock_fd, Client *head, Cred *creds, int no_of_users)
{
    Client *client = (Client *)malloc(sizeof(Client));
    client->username = NULL;
    client->password = NULL;
    client->sock_fd = sock_fd;

    client->creds = creds;
    client->no_of_users = no_of_users;

    client->head = head;
    client->next = NULL;
    return client;
}

Client *initialize_list()
{
    Client *head = new_client(0, NULL, NULL, 0);
    return head;
}

void add_client(Client *head, Client *client)
{
    Client *curr = head;
    while (curr->next)
        curr = curr->next;
    curr->next = client;
}

int remove_client(Client *head, Client *client)
{
    Client *curr = head;

    while (curr)
    {
        if (curr->next == client)
        {
            curr->next = client->next;
            free(client);
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int check_receipient(char *email, Cred *creds, int no_of_users)
{
    char username[50];
    int p = has_char(email, '@');
    if (p == -1)
        return -1;
    strncpy(username, email, p);
    username[p] = 0;
    for (int i = 0; i < no_of_users; i++)
    {
        if (strcmp(creds[i].username, username) == 0)
            return i;
    }

    return -1;
}

void get_time_string(char *time_string)
{
    time_t current_time;
    struct tm *tm_info;

    current_time = time(NULL);
    tm_info = gmtime(&current_time);
    strftime(time_string, 100, "%a, %d %b %Y %I:%M:%S GMT", tm_info);
}

void *handle_client(void *arg)
{
    Client *client = (Client *)arg;

    printf("- New client connected!\n");

    char *username, *password;
    if (recv_packet(client->sock_fd, &username) == -1)
    {
        printf("! received data was corrupted\n");
        printf("- Closing connection\n");
        close(client->sock_fd);
        remove_client(client->head, client);
        return NULL;
    }
    if (recv_packet(client->sock_fd, &password) == -1)
    {
        printf("! received data was corrupted\n");
        printf("- Closing connection\n");
        close(client->sock_fd);
        remove_client(client->head, client);
        return NULL;
    }

    char *invalid_password = "Invalid Password!";
    char *user_not_found = "User not found!";
    char *authenticated = "AUTHENTICATED";

    int flag = 0;
    for (int i = 0; i < client->no_of_users; i++)
    {
        if (strcmp(client->creds[i].username, username) == 0)
        {
            if (strcmp(client->creds[i].password, password) != 0)
            {
                send_packet(client->sock_fd, invalid_password, strlen(invalid_password));

                close(client->sock_fd);
                remove_client(client->head, client);
                return NULL;
            }
            else
            {
                flag = 1;
                client->username = username;
                client->password = password;
                break;
            }
        }
    }

    if (flag == 0)
    {
        send_packet(client->sock_fd, user_not_found, strlen(user_not_found));
        close(client->sock_fd);
        remove_client(client->head, client);
        return NULL;
    }

    send_packet(client->sock_fd, authenticated, strlen(authenticated));

    printf("- Client %s authenticated\n", client->username);

    while (1)
    {
        char *data;
        char buffer[1024];
        int len = recv_packet(client->sock_fd, &data);
        int i = 0;
        if (len == -1)
        {
            printf("! received data was corrupted\n");
            break;
        }
        if (strcmp(data, "EXIT") == 0)
        {
            break;
        }

        printf("- Received new mail\n");

        char from[50], to[50], subject[50];
        char body[1024];

        get_field(data, "From", from);
        get_field(data, "To", to);
        get_field(data, "Subject", subject);
        get_field(data, "Body", body);

        printf("> From: %s\n", from);
        printf("> To: %s\n", to);

        if (!verify_email(from))
        {
            char *invalid_from = "Invalid sender email\n";
            printf("! Invalid sender email\n");
            send_packet(client->sock_fd, invalid_from, strlen(invalid_from));
            continue;
        }

        if (!verify_email(to))
        {
            char *invalid_to = "Invalid recepient email\n";
            printf("! Invalid recepient email\n");
            send_packet(client->sock_fd, invalid_to, strlen(invalid_to));
            continue;
        }

        if ((i = check_receipient(to, client->creds, client->no_of_users)) == -1)
        {
            printf("! Recepient %s not found\n", to);
            char *email_error = "Invalid Email";
            send_packet(client->sock_fd, email_error, strlen(email_error));
            continue;
        }

        char mailbox_filename[100];
        sprintf(mailbox_filename, "%s/mymailbox.mail", client->creds[i].username);
        FILE *mailbox = fopen(mailbox_filename, "a");

        char time_string[50];
        get_time_string(time_string);

        fprintf(mailbox, "From: %s\n", from);
        fprintf(mailbox, "To: %s\n", to);
        fprintf(mailbox, "Subject: %s\n", subject);
        fprintf(mailbox, "Received: %s\n", time_string);
        fprintf(mailbox, "%s", body);

        fclose(mailbox);

        char *success = "EMAIL SENT";
        printf("- New mail sent to %s from %s by %s\n", to, from, client->username);

        send_packet(client->sock_fd, success, strlen(success));

        free(data);
    }

    printf("- Disconnecting client %s\n", client->username);

    close(client->sock_fd);

    remove_client(client->head, client);
}

void show_usage()
{
    printf("Usage: ");
    printf("./server.o PORT\n");
    printf("PORT is the port number on which the SMTP server should start\n");
}

int main(int argc, char *argv[])
{
    int port = PORT;
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
    int server_fd, conn_socket, recv_len;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char message[256];
    char buffer[1024] = {0};

    printf("Starting SMTP server...\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nError creating socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("Error setting socket options!\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Error binding to port!\n");
        return -1;
    }

    if (listen(server_fd, 3) < 0)
    {
        printf("Error while listening for connections!\n");
        return -1;
    }

    printf("Server started!\n");
    printf("Listening on 0.0.0.0:%d\n", port);

    char username[50], password[50];

    FILE *cred_file = fopen("logincred.txt", "r");

    char fusername[50], fpassword[50];

    int no_of_users = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        no_of_users++;

    Cred *creds = calloc(no_of_users, sizeof(Cred));

    fseek(cred_file, 0, SEEK_SET);
    int i = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
    {
        strcpy(creds[i].username, fusername);
        strcpy(creds[i].password, fpassword);
        i++;
    }

    Client *head = initialize_list();

    while (1)
    {
        if ((conn_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            printf("Error accepting connection!");
            continue;
        }

        fflush(stdout);

        Client *client = new_client(conn_socket, head, creds, no_of_users);
        add_client(head, client);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client);
    }

    return 0;
}