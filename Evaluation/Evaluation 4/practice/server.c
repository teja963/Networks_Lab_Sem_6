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
    if(inet_pton(AF_INET, "127.0.0.2", &(serv_addr.sin_addr)) <= 0) {
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
            // while(1) {
                temp = recv(conn_fd, buffer, sizeof(buffer), 0);
                printf("TEMP: %d\n", temp);
                if(check(conn_fd, "Receive", temp))
                    exit(0);

                char method[8], route[128], version[16];
                sscanf(buffer, "%s %s %s", method, route, version);
                
                printf("%s %s %s\n\n", method, route, version);
                printf("MESSAGE: \n%s\n", buffer);

                if(strcmp("GET",  method) == 0) {
                    printf("GET METHOD\n");
                    get(conn_fd, route);
                        // break;
                }
                else if(strcmp("POST", method) == 0) {
                    printf("POST METHOD\n");
                    post(conn_fd, route);
                        // break;
                }
                else {
                    printf("UNKNOWN METHOD\n");
                }
            // }

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
        // perror(str);
        // close(conn_fd);
        return 1;
    }
    return 0;
}

int get(int conn_fd, char *route) {
    char buffer[2048], buffer_1[2048];
    int temp; 
    FILE *fp = NULL;

    
    if(strcmp(route, "/") == 0) {
        printf("INDEX PAGE\n");
        fp = fopen("index.html", "r");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
        
    } 
    else if(strcmp(route, "/home") == 0) {
        printf("HOME PAGE\n");
        fp = fopen("home.html", "r");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }
    // else if(strcmp(route, ))
    else if(strcmp(route, "/favicon.ico") == 0) {
        printf("FAVICON.ICO\n");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: */*\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\nContent-Length: 0\r\n\r\n");
    }
    else if(strcmp(route, "/login.js") == 0) {
        fp = fopen("login.js", "r");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: application/javascript\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }
    else if(strcmp(route, "/nitc.png") == 0) {
        printf("NITC IMAGE\n");
        fp = fopen("instagram.jpg", "r");
        strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\n\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }
    else {
        printf("UNKNOWN ROUTE\n");
        strcpy(buffer, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }

    if(fp)
        while(fgets(buffer_1, sizeof(buffer_1) - 1, fp)) {
            // printf("TESTING: %s\n", buffer_1);
            strcat(buffer, buffer_1);
        }
        
    temp = send(conn_fd, buffer, strlen(buffer), 0);

    printf("%s\n", buffer);
    printf("*********************************\n");
    printf("TEMP: %d\n", temp);
    printf("*********************************\n");

    return check(conn_fd, "Send", temp);
}

int post(int conn_fd, char *route) {
    char buffer[2048], buffer_1[2048];
    int temp; 
    FILE *fp;

    char *data = route;
    for(; strncmp(data, "\r\n\r\n", 4) != 0; data++);
    data += 4;
    printf("DATA : %s %ld\n", data, strlen(data));

    
    if(strcmp(route, "/") == 0) {
        printf("INDEX PAGE\n");

        data += 9;
        char username[128] = "", password[128] = "";
        int i;
        for(i = 0; *data != '&'; i++, data++) 
            username[i] = *data;
        username[++i] = '\0';

        data += 10;
        strcpy(password, data);
        printf("USERNAME : %s\nPASSWORD: %s\n", username, password);

        if(authenticate(username, password)) {
            fp = fopen("home.html", "r");
            strcpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
        }
        else {
            fp = fopen("index.html", "r");
            strcpy(buffer, "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
        }     
    } 
    else {
        printf("UNKNOWN ROUTE\n");
        fp = fopen("default.html", "r");
        strcpy(buffer, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\nConnection: close\r\nServer: HTTP Server(By Gnani)\r\n\r\n");
    }

    while(fgets(buffer_1, sizeof(buffer_1) - 1, fp))
            strcat(buffer, buffer_1);
        
    temp = send(conn_fd, buffer, strlen(buffer), 0);
    return check(conn_fd, "Send", temp);
}

int authenticate(char *username, char *password) {
    // if(strcmp(username, "gnanesh") == 0 && strcmp(password, "123") == 0)
    if(strcmp(username, "gnanesh") == 0)
        return 1;
    return 0;
}