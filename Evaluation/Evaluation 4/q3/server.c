#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int check();
int get();
int post();
int authenticate();

#define PORT 8080

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1) {
        perror("Socket");
        exit(0);
    }

    printf("[+]SOCKET CREATED.........................................\n");

    struct sockaddr_in serv_addr, client_addr;
    int serv_addr_len = sizeof(serv_addr), client_addr_len = sizeof(client_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr)) <= 0) {
        perror("Addr. Conversion");
        exit(0);
    }

    if(bind(sock_fd, (struct sockaddr *)&serv_addr, serv_addr_len) == -1) {
        perror("Bind");
        exit(0);
    }

    printf("[+]SUCCESSFULLY BINDED....................................\n");

    if(listen(sock_fd, 5) == -1) {
        perror("Listen");
        exit(0);
    }

    printf("[+]LISTENING..............................................\n\n");

    int temp, conn_fd, pid;
    char buffer[2048],l;
    for(;;) {
        conn_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(conn_fd == -1) {
            perror("Accept");
            continue;
        }

        printf("[+]CLIENT(%d) CONNECTED.......................................\n", conn_fd);

        pid = fork();
        if(pid == -1) {
            perror("Fork");
            printf("[-]CONNECTION(%d) CLOSED......................................\n", conn_fd);
            close(conn_fd);
            continue;
        }
        else if(pid == 0) {
            //CHILD PROCESS

            temp = recv(conn_fd, buffer, sizeof(buffer), 0);
            if(check(conn_fd, "Receive", temp))
                exit(0);

            char method[8], route[128], version[16];
            sscanf(buffer, "%s %s %s", method, route, version);
            
            printf("%s %s %s\n\n", method, route, version);
            printf("MESSAGE: \n%s\n", buffer);

            if(strcmp("GET",  method) == 0) {
                printf("GET METHOD\n");
                get(conn_fd, route);
            }
            // else if(strcmp("POST", method) == 0) {
            //     printf("POST METHOD\n");
            //     post(conn_fd, route);
            // }
            else {
                printf("UNKNOWN METHOD\n");
                strcpy(buffer, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
                temp = send(conn_fd, buffer, strlen(buffer), 0);
            }

            close(conn_fd);
            printf("[-]CONNECTION CLOSED GRACEFULLY................................\n\n");
            exit(0);
        }
        else {
            //PARENT PROCESS
            close(conn_fd);
        }
    }
    close(sock_fd);
}

int check(int conn_fd, char *str, int temp) {
    if(temp == 0) {
        printf("[-]CONNECTION CLOSED(%s)(%d)......................................\n", str, conn_fd);
        // close(conn_fd);
        return 1;
    }
    else if(temp == -1) {
        printf("[-]ERROR(%s)(%d)..................................................\n", str, conn_fd);
        perror(str);
        // close(conn_fd);
        return 1;
    }
    return 0;
}

int get(int conn_fd, char *route) {
    char buffer[2048], buffer_1[2048];
    int temp; 
    FILE *fp = NULL;

    if((strcmp(route, "/mypage.html") == 0) || (strcmp(route, "/mypage") == 0)) {
        printf("INDEX PAGE\n");
        fp = fopen("mypage.html", "r");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
        
    } 
    else {
        printf("UNKNOWN ROUTE\n");
        // fp = fopen("default.html", "r");
        strcpy(buffer, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }

    if(fp)
        while(fgets(buffer_1, sizeof(buffer_1) - 1, fp))
            strcat(buffer, buffer_1);
        
    temp = send(conn_fd, buffer, strlen(buffer), 0);

    return check(conn_fd, "Send", temp);
}