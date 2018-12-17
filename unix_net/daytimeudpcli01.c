#include "unp.h"

int main(int argc, char **argv)
{
    int     sockfd, n;
    char    recvline[MAXLINE];
    socklen_t len;
    sockaddr    *addr;
    
    if (argc != 3)
    {
        printf("usage:%s <host> <service>\n", argv[0]);
        exit(0);
    }
    
    sockfd = udp_client(argv[1], argv[2], &addr, &len);
    
    sendto(sockfd, " ", 2, 0, addr, len);
    n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
    recvline[n] = 0;
    write(STDOUT_FILENO, recvline, n);
    exit(0);
}
