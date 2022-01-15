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
    char buffer[2048], buffer_1[2048], buffer_2[2048], username[1024];

    conn_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if(conn_fd == -1) {
        perror("Accept");
        exit(0);
    }

    printf("\n[+]CLIENT CONNECTED..................................\n");

    while(1) {
        bzero(username, sizeof(username));
        temp = recv(conn_fd, username, sizeof(username), 0);

        if(strncmp(username, "goodbye", strlen("goodbye")) == 0)
            break;

        bzero(buffer_2, sizeof(buffer_2));
        sprintf(buffer_2, "%s/mymailbox.mail", username);
        FILE *fp = fopen(buffer_2, "r");

        bzero(buffer, sizeof(buffer));
        bzero(buffer_1, sizeof(buffer_1));
        if(fp) 
            while(fgets(buffer_1, sizeof(buffer_1) - 1, fp)) {
                printf("%s", buffer_1);
                strcat(buffer, buffer_1);
            }
        else
            printf("FILE NOT FOUND\n");

        temp = send(conn_fd, buffer, strlen(buffer), 0);
    }

    printf("[-]CONNECTION CLOSED.................................\n");
        
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
