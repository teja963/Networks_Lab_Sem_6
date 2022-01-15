#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<unistd.h>

void type_and_domain();
void cache();

char *iterative_search();
char *find_server();

int check_cache();
int type_check();

int main(int argc,char *argv[]){
    char final[1024], domain[1024], type[10];
    int PORT = atoi(argv[1]);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd == -1){
        perror("Socket");
        exit(1);
    }

    printf("Socket Created................................\n");

    struct sockaddr_in serv_addr, client_addr;

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(PORT); 

    if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Bind");
        exit(1);
    }

    printf("Bind Successful...............................\n");
    printf("Listening.....................................\n");

    int client_addr_len,n;
    char buffer[1024];

    client_addr_len = sizeof(client_addr);
    while(1) {
        char cache_buffer[2048];
        bzero(buffer, sizeof(buffer));
        
        n = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        
        if(strncmp(buffer, "quit", 4) == 0) 
            exit(0);

        type_and_domain(buffer, type, domain);

        printf("Domain: %s\n", domain);
        printf("Type: %s\n", type);

        if(check_cache(cache_buffer, type, domain)) {
            printf("Answer found in cache\n");
            sendto(sock_fd, (char *)cache_buffer, strlen(cache_buffer),  MSG_CONFIRM, (struct sockaddr *)&client_addr, client_addr_len);      
        }
        else if(type_check(type))
            sendto(sock_fd, "UNAUTHORISED REQUEST", 20, MSG_CONFIRM, (struct sockaddr *) &client_addr, client_addr_len); 
        else {
            char split[10][1024];
            char result_cmd[1024];
            char buffer_1[1024];
        
            strcpy(split[0], domain);
            strcpy(buffer_1, domain);
        
            char delim[2] = ".";
            char *token = strtok(buffer_1, delim);

            int i = 1;
            while(token != NULL) {
                strcpy(split[i++], token);
                token = strtok(NULL, delim);
            }

            n = i;

            char step_resolve[n][1024]; 
            strcpy(step_resolve[0], split[n - 1]);

            int j = 1;
            for(i = n - 2; i > 0; i--, j++) {
                strcpy(step_resolve[j], split[i]);
                strcat(step_resolve[j], ".");
                strcat(step_resolve[j], step_resolve[j - 1]);
            }

            n--;
            iterative_search(".", final);
            for(i = 1; i < n; i++) 
                iterative_search(step_resolve[i], final);

            strcpy(result_cmd, "nslookup -type=");
            strcat(result_cmd, type);
            strcat(result_cmd, " ");
            strcat(result_cmd, step_resolve[n-1]);
            strcat(result_cmd, " ");
            strcat(result_cmd, final);
            system(": > out.txt");
            strcat(result_cmd,">> out.txt");

            system(result_cmd);
            
            bzero(buffer, sizeof(buffer));
            bzero(buffer_1, sizeof(buffer_1));

            FILE *fp = fopen("out.txt", "r");
            while(fgets(buffer_1, sizeof(buffer_1), fp))
                strcat(buffer, buffer_1);

            n = sendto(sock_fd, buffer, strlen(buffer),  0, (struct sockaddr *)&client_addr, client_addr_len); 
            fclose(fp);

            cache(buffer, type, domain);

            printf("Written to cache\n");
    
        }
        printf("-------------\n");
    }
    close(sock_fd);

}

void type_and_domain(char *line, char *type, char *domain) {
    char delim[2] = "=";
    char *token;

    token = strtok(line, delim);
    token = strtok(NULL, delim);
 
    bzero(type, sizeof(type));
    bzero(domain, sizeof(domain));

    sscanf(token, "%s %s", type, domain);
}

void cache(char *text, char *type, char *domain) {
    FILE *fp = fopen("cache.txt", "a");
    fprintf(fp, "%s-%s\n", domain, type);
    fprintf(fp, "%s\n", text);
    fprintf(fp, "--------------------------\n");
    fclose(fp);
}

char *iterative_search(char *domain, char *final) {
    system(": > out.txt");

    char command[512];

    strcpy(command, "nslookup -type=ns ");
    strcat(command, domain);
    strcat(command," >> out.txt");

    system(command);

    FILE *fp = fopen("out.txt", "r");

    char buffer[1024];
    int len = strlen(domain);
    int i = 0;
    char ch;

    while(fgets(buffer, sizeof(buffer) - 1, fp))
        if(strncmp(buffer, domain, strlen(domain)) == 0) {
            char server[1024];
            strcpy(server, find_server(buffer));
            strcpy(final, server);
            break;
        }
    
    fclose(fp);    
}

char *find_server(char *line) {
    char delim[3] = "= ";
    char *token;

    token = strtok(line, delim);
    token = strtok(NULL, delim);

    if(strncmp(token, "nam", 3) == 0)
        token=strtok(NULL,delim);

    token[strlen(token) - 1] = '\0';
    return token;
}

int check_cache(char *cache_buffer, char *type, char *domain){
    FILE *fp = fopen("cache.txt", "r");
    char check[512];

    strcpy(check, domain);
    strcat(check, "-");
    strcat(check, type);

    char buffer[1024];
    bzero(cache_buffer, sizeof(cache_buffer));

    while(fgets(buffer, sizeof(buffer), fp))
        if(strncmp(buffer, check, strlen(check)) == 0) {
            while(1) {
                bzero(buffer, sizeof(buffer));
                fgets(buffer, 1024, fp);
                if(strncmp(buffer, "---", 3) == 0) 
                    break;
                strcat(cache_buffer, buffer);
            }
            return 1;
        }
    return 0;
}

int type_check(char *type) {
    if((strcmp(type, "a") == 0) || (strcmp(type, "aaaa") == 0) || (strcmp(type, "ns") == 0) || (strcmp(type, "cname") == 0)) 
        return 0;
    return 1;
}