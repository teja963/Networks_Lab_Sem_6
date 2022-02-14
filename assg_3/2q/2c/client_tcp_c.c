#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
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

int sockfd, connfd;
struct sockaddr_in servaddr, cli;
char buffer[1024];

int frame_id = 0;
Frame frame_send;
Frame frame_recv;
int ack_recv = 1;
int f_recv_size;

void sig_handler(int signum)
{
    printf("[-]Ack Not Received\n");
    write(sockfd, &frame_send, sizeof(frame_send));
    alarm(1);
    printf("[+]Frame Send\n");
}

int main()
{
    clock_t t;

    signal(SIGALRM, sig_handler);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }

    FILE *fp = fopen("sample.txt", "r");
    int n, data = 0;

    while (data < 1000)
    {

        if (ack_recv == 1)
        {
            data += 200;
            n = fread(buffer, 1, data, fp);
        }
        frame_send.sq_no = frame_id;
        frame_send.frame_kind = 1;
        frame_send.ack = 0;
        strcpy(frame_send.packet.data, buffer);

        write(sockfd, &frame_send, sizeof(frame_send));
        t = clock();
        alarm(1);
        printf("[+]Frame Send. Data Size: %d bytes\n", n);

        f_recv_size = read(sockfd, &frame_recv, sizeof(frame_recv));
        alarm(0);
        t = clock() - t;

        if (f_recv_size > 0 && frame_recv.sq_no == 0 && frame_recv.ack == frame_id + 1)
        {
            printf("[+]Ack Received\n");
            printf("[+]RTT : %f\n", ((double)t / CLOCKS_PER_SEC));
            ack_recv = 1;
        }

        frame_id++;
    }

    fclose(fp);
    frame_send.sq_no = frame_id;
    frame_send.frame_kind = 2;
    frame_send.ack = 0;
    frame_send.packet.data[0] = '\0';
    write(sockfd, &frame_send, sizeof(frame_send));

    close(sockfd);
    return 0;
}
