#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include <fcntl.h>
void main(int argc, char *argv[])
{
    
    if(argc<3)//If IP address is not provided
    {
        printf("Server IP address and Username not provided.\n");
        exit(1);
    }
    int sockfd,portno=3000 ,n;
    char ibuffer[255],obuffer[255];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    struct hostent  *server;
    fd_set input;
    FD_ZERO(&input);
    
    struct timeval timeout;
    char name[24];
    timeout.tv_sec  = 1;
    timeout.tv_usec = 1000;
    sockfd = socket(AF_INET,SOCK_STREAM,0);//Create a socket with IPv4 and TCP connection
    if (sockfd<0)//Socket Creation Failed
    {
        perror("Socket Error");
        exit(1);     
    }
    server = gethostbyname(argv[1]);//Convert the IP address of server passed into a hostent object
    if(server==NULL)//If conversion failed
    {
        fprintf(stderr,"Host error\n");
        exit(1);
         
    }
    bzero((char *)&server_addr,sizeof(server_addr));//Clear the server address struct object
    server_addr.sin_family = AF_INET;//Set the sin family to IPv4
    bcopy((char *)server->h_addr,(char*)&server_addr.sin_addr.s_addr,server->h_length); //Copy the server IP address from the hostent object to the server address object.

    server_addr.sin_port=htons(portno);//Set the destination port

    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)//Connect the socket to the destination IP address using TCP
    {
        perror("Connection Error");
        exit(1);
    }
    char newline[]="\n";

   
    printf("Enter the passwd: ");
    char pword[25];
    bzero(pword,sizeof(pword));
    fgets(pword,sizeof(pword),stdin);
    pword[strcspn(pword,newline)] = '\0';
    n = write(sockfd,pword,strlen(pword));
   // printf("Wrote\n");
    if(n<0)//Write failed
    {
        perror("error writing client");
        exit(1);
    }
    
    strcpy(name,argv[2]);
    char response[25];
    n= read(sockfd,response,sizeof(response));
   // printf("Recieved flag: %s\n",response);
    if(strcmp(response,"Accepted")==0)
    {
    //printf("joined: %s",name);
    n = write(sockfd,name,strlen(name));//Write the accepted message into the socket
    printf("\nWelcome to Project COBRA\n");
    int flags=fcntl(0,F_GETFL);
    flags |=O_NONBLOCK;
    fcntl(0,F_SETFL,flags);
    while(1)
    {
        bzero(ibuffer,sizeof(ibuffer));
       // fprintf(stdout,">");
        fgets(ibuffer,sizeof(ibuffer),stdin);//Accept message from the keyboard
       // printf("Here1\n");
        ibuffer[strcspn(ibuffer,newline)] = '\0';
        n = write(sockfd,ibuffer,strlen(ibuffer));//Write the accepted message into the socket
        if(n<0)//Write failed
        {
            perror("error writing client");
            exit(1);
        }
        if(strncmp("bye",ibuffer,3)==0)
        {
            printf("You left the Discussion\n");
            break;
        }
        bzero(ibuffer,sizeof(ibuffer));
       // printf("Here3\n");
        FD_SET(sockfd, &input);
        n= select(sockfd+1,&input,NULL,NULL,&timeout);//Read message from the socket
        if(n==0)
        continue;
        else
        {   
            bzero(obuffer,sizeof(obuffer));
             n=read(sockfd,obuffer,sizeof(obuffer));
             
        }
        if(n<0)
        {
            perror("error Reading client");
            exit(1);
        }
        fprintf(stdout,"%s\n",obuffer);
        
    }
    }
    else printf("Incorrect Password!!\n");
    close(sockfd); //Close the socket
}