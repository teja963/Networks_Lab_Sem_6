#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

// #define PORT 25
#define PORT 8080
// #define PORT_1 110
#define PORT_1 8081

int search();
void print_msg();
int filter_msg();

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1) {
        perror("Socket");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    int serv_addr_len = sizeof(serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr)) <= 0) {
        perror("Addr. Conversion");
        exit(0);
    }

    if(connect(sock_fd, (struct sockaddr *)&serv_addr, serv_addr_len) == -1) {
        perror("Connect");
        exit(0);
    }

    int temp;
    char buffer[2048], username[128];

    // 220
    temp = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
    printf("SERVER: %s", buffer);

    // EHLO
    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "EHLO\r\n");
    temp = send(sock_fd, buffer, strlen(buffer), 0);

    // 250 OK
    bzero(buffer, sizeof(buffer));
    temp = recv(sock_fd, buffer, sizeof(buffer), 0);
    
    do {
        // AUTH LOGIN
        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "AUTH LOGIN\r\n");
        temp = send(sock_fd, buffer, strlen(buffer), 0);

        // 334 Username: 
        bzero(buffer, sizeof(buffer));
        temp = recv(sock_fd, buffer, sizeof(buffer), 0);
        printf("USERNAME: ");

        // User
        bzero(username, sizeof(username));
        bzero(buffer, sizeof(buffer));
        scanf("%s", username);
        sprintf(buffer, "%s\r\n", username);
        temp = send(sock_fd, buffer, strlen(buffer), 0);

        // 334 Password 
        bzero(buffer, sizeof(buffer));
        temp = recv(sock_fd, buffer, sizeof(buffer), 0);
        printf("PASSWORD: ");

        // Pass
        bzero(buffer, sizeof(buffer));
        scanf("%s", buffer);
        strcat(buffer, "\r\n");
        temp = send(sock_fd, buffer, strlen(buffer), 0);

        // 235 Authentication Succeeded || 535 Authentication Credentials Invalid
        bzero(buffer, sizeof(buffer));
        temp = recv(sock_fd, buffer, sizeof(buffer), 0);
        printf("SERVER: %s", buffer);

    } while(strncmp(buffer, "235", 3) != 0);

    char from[128], to[128], subject[128], line[128];
    char msg[1024];
    char buffer_1[160], buffer_2[160], buffer_3[160], buffer_4[160], buffer_5[160];

    for(;;) {
        printf("COMMANDS AVAILABLE:\n1)Send - To send mail\n2)Manage - Shows the received mails\n3)Quit - Close the application\n");
        bzero(buffer, sizeof(buffer));
        scanf("%s", buffer);

        temp = strlen(buffer);
        buffer[temp] = '\r';
        buffer[temp + 1] = '\n';
        temp = send(sock_fd, buffer, strlen(buffer), 0);

        if(strcmp(buffer, "Quit\r\n") == 0)
            break;
        else if(strcmp(buffer, "Manage\r\n") == 0) {
            //To read newline character from scanf
            getchar();
            int pop_fd = socket(AF_INET, SOCK_STREAM, 0);
            if(pop_fd == -1) {
                perror("Socket");
                continue;
            }

            struct sockaddr_in serv_addr_1;
            int serv_addr_len_1 = sizeof(serv_addr_1);

            serv_addr_1.sin_family = AF_INET;
            serv_addr_1.sin_port = htons(PORT_1);
            if(inet_pton(AF_INET, "127.0.0.1", &(serv_addr_1.sin_addr)) <= 0) {
                perror("Addr. Conversion");
                continue;
            }

            if(connect(pop_fd, (struct sockaddr *)&serv_addr_1, serv_addr_len_1) == -1) {
                perror("Connect");
                continue;
            }

            char ch;
            int flag = 0;
            int count = 0;
            // temp = send(pop_fd, username, strlen(username), 0);

            while(1) {
                
                temp = send(pop_fd, username, strlen(username), 0);

                bzero(buffer, sizeof(buffer));
                bzero(buffer_1, sizeof(buffer_1));

                temp = recv(pop_fd, buffer, sizeof(buffer), 0);

                strcpy(buffer_1, buffer);
                if(temp) {
                    print_msg(buffer_1, buffer);
                    printf("%s", buffer_1);
                }

                printf("Enter Sender Mail Address: ");
                scanf("%s", buffer_2);
                getchar();

                if(!strcmp(buffer_2, "a@localhost") && !strcmp(buffer_2, "Stalling@localhost") && !strcmp(buffer_2, "Fourouzan@localhost") && !strcmp(buffer_2, "Andrew@localhost"))
                    printf("INVALID MAIL-ID\n");

                if(!filter_msg(buffer, buffer_2))
                    printf("NO MAILS FOUND\n");

                printf("Enter \"q\" To Quit Else Enter Some Other Character:\n");
                scanf("%c", &ch);
                if(ch == 'q') {
                    getchar();
                    strcpy(buffer, "goodbye\r\n");
                    temp = send(pop_fd, buffer, strlen(buffer), 0);
                    break;
                }

                flag = 0;
            }
            close(pop_fd);
        }
        else if(strcmp(buffer, "Send\r\n") == 0) {
            //To read newline character from scanf
            getchar();
            printf("Enter your mail\n");

            bzero(msg, sizeof(msg));
            bzero(line, sizeof(line));

            for(int i = 0; fgets(line, 80, stdin) && i < 80; i++, bzero(line, sizeof(line))) {
                temp = strlen(line);
                line[temp - 1] = '\r';
                line[temp] = '\n';
                strcat(msg, line);

                if(!strncmp(line, ".\r\n", 3))
                    break;
            }

            bzero(from, sizeof(from));
            bzero(to, sizeof(to));
            sscanf(msg, "%s %s\r\n%s %s\r\n%s", buffer_1, from, buffer_2, to, buffer_3);

            if(!strcmp(buffer_1, "From:") && !strcmp(buffer_2, "To:") && !strcmp(buffer_3, "Subject:") && search(from) && search(to) && !strncmp(username, from, strlen(username))) {
                // printf("%ld %ld %ld %ld %ld\n", strlen(buffer_1), strlen(from), strlen(buffer_2), strlen(to), strlen(buffer_3));

                // MAIL FROM
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "MAIL FROM: <%s>\r\n", from);
                temp = send(sock_fd, buffer, strlen(buffer), 0);

                // 250 OK
                bzero(buffer, sizeof(buffer));
                temp = recv(sock_fd, buffer, sizeof(buffer), 0);

                // RCPT TO
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "RCPT TO: <%s>\r\n", to);
                temp = send(sock_fd, buffer, strlen(buffer), 0);

                // 250 OK
                bzero(buffer, sizeof(buffer));
                temp = recv(sock_fd, buffer, sizeof(buffer), 0);

                // DATA
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "DATA\r\n");
                temp = send(sock_fd, buffer, strlen(buffer), 0);

                // 354 Start Mail Input
                bzero(buffer, sizeof(buffer));
                temp = recv(sock_fd, buffer, sizeof(buffer), 0);

                // MESSAGE
                temp = send(sock_fd, msg, strlen(msg), 0);

                // 250 OK
                bzero(buffer, sizeof(buffer));
                temp = recv(sock_fd, buffer, sizeof(buffer), 0);
                
                if(!strncmp(buffer, "250", 3))
                    printf("Mail sent successfully\n");
            }
            else {
                printf("Invalid Format\n");
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "RSET Invalid Message Format\r\n");
                temp = send(sock_fd, buffer, strlen(buffer), 0);
            }
        }
        else {
            printf("Invalid Command\n");
        }

        
    }

    close(sock_fd);
}

int search(char *str) {
    for(int i = 0; i < strlen(str); i++) 
        if(str[i] == '@')
            return 1;

    return 0;
}

void print_msg(char *dest, char *src) {

    int state = 0;
    while(*src != '\0') {

        //From: 
        if(state == 0) {
            if(*src == '\r') {
                src += 1;
                state = 4;
                *dest = '\n';
                dest++;
            }
            else {
                *dest = *src;
                dest++;
            }
        }
        //Subject: 
        else if(state == 1) {
            if(*src == '\r') {
                src += 1;
                state = 2;
                *dest = '\n';
                dest++;
            }
            else {
                *dest = *src;
                dest++;
            }
        }
        //Received
        else if(state == 2) {
            if(*src == '\n') {
                src += 2;
                state = 3;
                *dest = '\n';
            }
            else {
                *dest = *src;
                dest++;
            }
        }
        else if(state == 3) {
            if(*src == '\r') {
                src += 1;
                if(*(src + 1) == '.') {
                    src += 2;
                    state = 0;
                }
            }
        }
        //To
        else if(state == 4) {
            if(*src == '\r') {
                src += 1;
                state = 1;
            }
        }
        src++;
    }
    *dest = '\0';
}

int filter_msg(char *src, char *mail) {
    int ret = 0;
    // int state = 0;
    // while(*src != '\0') {

    //     //From: 
    //     if(state == 0) {
    //         if(*src == '\r') {
    //             src += 1;
    //             state = 4;
    //             printf("\n");
    //         }
    //         else {
    //            printf("%c", *src);
    //         }
    //     }
    //     //Subject: 
    //     else if(state == 1) {
    //         if(*src == '\r') {
    //             src += 1;
    //             state = 2;
    //             printf("\n");
    //         }
    //         else {
    //             printf("%c", *src);
    //         }
    //     }
    //     //Received
    //     else if(state == 2) {
    //         if(*src == '\n') {
    //             src += 2;
    //             state = 3;
    //             printf("\n");
    //         }
    //         else {
    //             printf("%c", *src);
    //         }
    //     }
    //     else if(state == 3) {
    //         if(*src == '\r') {
    //             src += 1;
    //             if(*(src + 1) == '.') {
    //                 src += 2;
    //                 state = 0;
    //                 printf("\n");
    //             }
                
    //         }
    //         printf("%c", *src);
    //     }
    //     //To
    //     else if(state == 4) {
    //         if(*src == '\r') {
    //             src += 1;
    //             state = 1;
    //             printf("\n");
    //         }
    //         printf("%c", *src);
    //     }
    //     src++;
    // }

    int state = 0;
    while(*src != '\0') {
        if(state == 0) {
            if(*src == '\0' | *(src + 1) == '\0')
                break;
            else if(!strncmp(src + 6, mail, strlen(mail))) {
                ret = 1;
                state = 1;
            }
            else 
                state = 2;
        }
        else if(state == 1) {
            if(*src == '\0')
                break;
            while(1) {
                printf("%c", *src);
                if(*src == '\r' && *(src + 2) == '.')
                    src += 5;
                else 
                    src++;
            }
            state = 0;
        }
        else {
            if(*src == '\0')
                break;
            while(1) {
                if(*src == '\r' && *(src + 2) == '.')
                    src += 5;
                else 
                    src++;
            }
            state = 0;
        }
    }

    return ret;
}