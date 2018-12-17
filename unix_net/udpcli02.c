//UDP回射 客户端 调用connect
#include "unp.h"

void dg_cli(int sockfdin, int sockfd);
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("usage:%s <IPADDRESS> <PORT>", argv[0]);
        exit(0);
    }
    
    int             sockfd;
    sockaddr_in     servaddr;
    
    memset(&servaddr, 0, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
    {
        perror("inet_pton error");
        exit(0);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error");
        exit(0);
    }
    
    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect error");
        exit(0);
    }
    
    dg_cli(STDIN_FILENO, sockfd);
    exit(0);
}

void dg_cli(int sockfdin, int sockfd)
{
    int     m, n;
    char    sendbuf[MAXLINE], recvbuf[MAXLINE];
    
    while ((m = read(sockfdin, sendbuf, sizeof(sendbuf))) > 0)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        printf("read %d bytes from stdin:%s\n", m, sendbuf);
        if (write(sockfd, sendbuf, m) < 0)
        {
            perror("write sockfd error");
            break;
        }
        else if ((n = read(sockfd, recvbuf, sizeof(recvbuf))) <= 0)
        {
            perror("read sock error");
            break;
        }
        else
        {
            printf("read %d bytes from socket %d:%s\n", n, sockfd, recvbuf);
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }
    
    if (m < 0)
        perror("read from stdin error:");
    else
        printf("read finished from stdin\n");
}