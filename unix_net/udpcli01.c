//UDPªÿ…‰ øÕªß∂À
#include "unp.h"

void dg_cli(int sockfdin, int sockfd, sockaddr* servaddr, socklen_t servlen);
void dg_cli2(int sockfdin, int sockfd, sockaddr* servaddr, socklen_t servlen);
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
    
    dg_cli(STDIN_FILENO, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    exit(0);
}

void dg_cli(int sockfdin, int sockfd, sockaddr* servaddr, socklen_t servlen)
{
    int     m, n;
    char    sendbuf[MAXLINE], recvbuf[MAXLINE];
    
    while ((m = read(sockfdin, sendbuf, sizeof(sendbuf))) > 0)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        sendto(sockfd, sendbuf, m, 0, servaddr, servlen);
        n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        write(STDOUT_FILENO, recvbuf, n);
        memset(sendbuf, 0, sizeof(sendbuf));
    }
    
    if (m < 0)
        perror("read from stdin error");
    else
        printf("read finished from stdin\n");
}

void dg_cli2(int sockfdin, int sockfd, sockaddr* servaddr, socklen_t servlen)
{
    int     i;
    char    sendbuf[MAXLINE];
    memset(sendbuf, 0, MAXLINE);
    memset(sendbuf, 1, 10);
    
    for (i = 0; i < 10000; ++i)
    {
        sendto(sockfd, sendbuf, sizeof(sendbuf), 0, servaddr, servlen);
    }
}