#include "unp.h"

int main(int argc, char **argv)
{
    int     sockfd, n;
    char    recvline[MAXLINE];
    
    if (argc != 3)
    {
        printf("usage:%s <host> <service>\n", argv[0]);
        exit(0);
    }
    
    sockfd = udp_connect(argv[1], argv[2]);
    
    write(sockfd, " ", 2);
    n = read(sockfd, recvline, MAXLINE);
    recvline[n] = 0;
    write(STDOUT_FILENO, recvline, n);
    exit(0);
}
