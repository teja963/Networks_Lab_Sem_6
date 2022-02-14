#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define SA struct sockaddr

typedef struct packet
{
    char data[1024];
} Packet;

typedef struct frame
{
    int frame_kind; // ACK:0, SEQ:1 FIN:2
    int sq_no;
    int ack;
    Packet packet;
} Frame;

int main()
{

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    int frame_id = 0;
    Frame frame_recv;
    Frame frame_send;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }

    len = sizeof(cli);

    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }
    char buffer[1024];

    while (1)
    {
        int f_recv_size = read(connfd, &frame_recv, sizeof(frame_recv));

        if (f_recv_size > 0 && frame_recv.sq_no == frame_id && frame_recv.frame_kind == 2)
            break;

        else if (f_recv_size > 0 && frame_recv.frame_kind == 1 && frame_recv.sq_no == frame_id)
        {
            printf("[+]Frame Received. Data size : %ld bytes\n", strlen(frame_recv.packet.data));

            frame_send.sq_no = 0;
            frame_send.frame_kind = 0;
            frame_send.ack = frame_recv.sq_no + 1;
            write(connfd, &frame_send, sizeof(frame_send));
            printf("[+]Ack Send\n");
        }
        else if (f_recv_size > 0 && frame_recv.frame_kind == 1 && frame_recv.sq_no < frame_id)
        {
            continue;
        }
        frame_id++;
    }

    close(sockfd);
    return 0;
}
