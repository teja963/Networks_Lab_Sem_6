#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int check();
int authenticate();
void extract_name();
void append_msg();

int main() {
    int my_port;
    printf("ENTER PORT NUMBER: ");
    scanf("%d", &my_port);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1) {
        perror("Socket");
        exit(0);
    }

    printf("[+]SOCKET CREATED.......................................\n");

    struct sockaddr_in serv_addr, client_addr;
    int serv_addr_len, client_addr_len;
    serv_addr_len = sizeof(serv_addr);
    client_addr_len = sizeof(client_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(my_port);
    if(inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr)) <= 0) {
        perror("Addr. Conversion");
        exit(0);
    }

    if(bind(sock_fd, (struct sockaddr *)&serv_addr, serv_addr_len) == -1) {
        perror("Bind");
        exit(0);
    }

    printf("[+]SUCCESSFULLY BINDED..................................\n");

    if(listen(sock_fd, 5) == -1) {
        perror("Listen");
        exit(0);
    }

    printf("[+]LISTENING............................................\n");

    int conn_fd, temp, pid;
    char buffer[2048], buffer_1[2048], buffer_2[2048];
    for(;;) {
        conn_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(conn_fd == -1) {
            perror("Accept");
            exit(0);
        }

        printf("\n[+]CLIENT(%d) CONNECTED..................................\n", conn_fd);
        pid = fork();
        if(pid == -1) {
            perror("Fork");
            printf("[-]CONNECTION(%d) CLOSED.................................\n", conn_fd);
            close(conn_fd);
        }
        else if(pid == 0) {
            // CHILD PROCESS

            char username[128], password[128];
            char from[128], to[128], msg[1024];
            char directory[128];
            FILE *fp = NULL;

            // 220
            bzero(buffer, sizeof(buffer));
            strcpy(buffer, "220 localhost service ready\r\n");
            temp = send(conn_fd, buffer, strlen(buffer), 0);

            // EHLO
            bzero(buffer, sizeof(buffer));
            temp = recv(conn_fd, buffer, sizeof(buffer) - 1, 0);

            // 250 OK
            bzero(buffer, sizeof(buffer));
            strcpy(buffer, "250 OK\r\n");
            temp = send(conn_fd, buffer, strlen(buffer), 0);

            while(1) {
                // AUTH LOGIN
                bzero(buffer, sizeof(buffer));
                temp = recv(conn_fd, buffer, sizeof(buffer) - 1, 0);

                // 334 Username
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "334 Username\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);

                // User
                bzero(username, sizeof(username));
                temp = recv(conn_fd, username, sizeof(username) - 1, 0);

                // 334 Password
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "334 Password\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);

                // Pass
                bzero(password, sizeof(password));
                temp = recv(conn_fd, password, sizeof(password) - 1, 0);

                if(authenticate(username, password)) {
                    // 235 Authenttication Succeeded
                    bzero(buffer, sizeof(buffer));
                    strcpy(buffer, "235 Authentication Succeeded\r\n");
                    temp = send(conn_fd, buffer, strlen(buffer), 0);

                    break;
                }
                else {
                    // 535 Authenttication Credential Invalid 
                    bzero(buffer, sizeof(buffer));
                    strcpy(buffer, "535 Authentication Credentials Invalid\r\n");
                    temp = send(conn_fd, buffer, strlen(buffer), 0);
                }
            }
            
            for(int i = 0;; i++) {
                // Send || Quit
                bzero(buffer, sizeof(buffer));
                temp = recv(conn_fd, buffer, sizeof(buffer), 0);
                if(!strcmp(buffer, "Quit\r\n"))
                    break;

                // MAIL FROM
                bzero(from, sizeof(from));
                temp = recv(conn_fd, from, sizeof(from) - 1, 0);
                if(check(conn_fd, "Receive", temp))
                    break;

                if(strncmp(from, "MAIL FROM", 9))
                    continue;

                printf("FROM: %s", from);

                // 250 OK
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "250 OK\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);

                // RCPT TO
                bzero(to, sizeof(to));
                temp = recv(conn_fd, to, sizeof(to) - 1, 0);
                if(check(conn_fd, "Receive", temp))
                    break;

                printf("TO: %s", to);

                // 250 OK
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "250 Accepted\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);

                // DATA
                bzero(buffer, sizeof(buffer));
                temp = recv(conn_fd, buffer, sizeof(buffer), 0);

                // 354 Start Mail Input
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "354 Start Mail Input\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);

                // MESSAGE
                bzero(msg, sizeof(msg));
                temp = recv(conn_fd, msg, sizeof(msg), 0);

                // Extracting the "directory" name from "to"
                bzero(directory, sizeof(directory));
                extract_name(directory, to);

                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "%s/mymailbox.mail", directory);

                // Trying to append the message
                fp = fopen(buffer, "a");
                if(fp == NULL) {
                    //553 
                    printf("FILE NOT FOUND\n");
                    bzero(buffer, sizeof(buffer));
                    strcpy(buffer, "553 Invalid Email Address\r\n");
                    temp = send(conn_fd, buffer, sizeof(buffer), 0);
                }
                else {
                    append_msg(fp, msg);

                    //250 OK
                    bzero(buffer, sizeof(buffer));
                    strcpy(buffer, "250 OK\r\n");
                    temp = send(conn_fd, buffer, sizeof(buffer), 0);
                } 
            }
            close(conn_fd);
            printf("[-]CONNECTION(%d) CLOSED GRACEFULLY......................\n", conn_fd);
            exit(0);
        }
        else
            close(conn_fd);
    }
    close(conn_fd);
}

int check(int conn_fd, char *str, int temp) {
    if(temp == 0) {
        close(conn_fd);
        printf("[-]CONNECTION(%d) CLOSED.................................\n", conn_fd);
        return 1;
    }
    else if(temp == -1) {
        perror(str);
        close(conn_fd);
        printf("[-]CONNECTION(%d) CLOSED.................................\n", conn_fd);
        return 1;
    }
    return 0;
}

int authenticate(char *username, char *password) {
    FILE *fp = fopen("logincred.txt", "r");
    
    int i, j, k, state = 0;
    char str[512], user[128], pass[128];
    while(fscanf(fp, "%s", str) != -1) {
        bzero(user, sizeof(user));
        bzero(pass, sizeof(pass));

        // Checking Username
        for(i = 0, j = 0; ; i++, j++){
            if(str[j] == ',') {
                user[i++] = '\r';
                user[i++] = '\n';
                user[i++] = '\0';
                j++;
                break;
            }
            user[i] = str[j];
        }

        // printf("%s\r %d\n", user, strcmp(username, user));

        if(strcmp(user, username))
            continue;

        // Checking Password
        for(i = 0; ; i++, j++){
            if(str[j] == '\0') {
                pass[i++] = '\r';
                pass[i++] = '\n';
                pass[i++] = '\0';
                break;
            }
            pass[i] = str[j];
        }

        // printf("%s\r %d\n", pass, strcmp(password, pass));

        if(!strcmp(pass, password)) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void extract_name(char *dest, char *src) {
    for(; *src != '<'; src++);
    src++;
    for(; *src != '@'; src++, dest++)
        *dest = *src;
    dest++;
    *dest = '\0';
}

void append_msg(FILE *fp, char *msg) {
    int state = 0;
    int count = 0;

    for(; count != 3; msg++) {
        fprintf(fp, "%c", *msg);
        if(!state && *msg == '\r')
            state = 1;
        else if(state && *msg == '\n') {
            state = 0;
            count++;
        }
        else
            state = 0;
    }

    struct tm *ptr;
    time_t lt;
    lt = time(NULL);
    ptr = localtime(&lt);

    fprintf(fp, "Received: %s\r\n", asctime(ptr));

    for(; *msg != '\0'; msg++)
        fprintf(fp, "%c", *msg);

    fclose(fp);
}
