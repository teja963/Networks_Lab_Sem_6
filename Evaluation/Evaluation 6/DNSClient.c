#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h> 

#define PORT 1234
  
int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd == -1) { 
        perror("Socket");
        exit(1); 
    } 

    char buffer[2048]; 
    struct sockaddr_in serv_addr; 

    printf("Socket created successfully..................................\n");      

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
      
    int serv_addr_len, temp; 
    serv_addr_len = sizeof(serv_addr);

    while(1) {
        printf("Enter your Query:");
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer) - 1, stdin);

        buffer[strlen(buffer)]='\0';
        temp = sendto(sock_fd, buffer, strlen(buffer) + 1, MSG_CONFIRM, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); 

        if(strncmp(buffer, "quit", 4) == 0) 
            break;
            
        bzero(buffer, sizeof(buffer));
        temp = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serv_addr, &serv_addr_len);  

        printf("%s", buffer);
        printf("\n-------\n");
    }
  
    close(sock_fd); 
    return 0; 
}