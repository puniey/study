#include "unp.h"

int main(int argc, char **argv)
{
    int     sockfd, n;
    char    recvline[MAXLINE];
    socklen_t   len;
    
    if (argc != 3)
    {
        printf("usage:%s <host> <service>\n", argv[0]);
        exit(0);
    }
    
    if ((sockfd = tcp_connect(argv[1], argv[2])) < 0)
    {
        exit(0);
    }
    
    while ((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        write(STDOUT_FILENO, recvline, n);
    }
    exit(0);
}