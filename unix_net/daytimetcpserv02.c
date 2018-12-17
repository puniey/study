#include "unp.h"

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    socklen_t len;
    char    buf[MAXLINE];
    time_t  ticks;
    sockaddr_in cliaddr;
    
    if (argc != 2)
    {
        printf("usage:%s <service>\n", argv[0]);
        exit(0);
    }
    
    if ((listenfd = tcp_listen(NULL, argv[1], NULL)) < 0)
        exit(0);
    
    while (1)
    {
        len     = sizeof(cliaddr);
        if ((connfd  = accept(listenfd, (sockaddr*)&cliaddr, &len)) < 0)
        {
            perror("accept error");
            break;
        }
        
        printf("connect from %s:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), ntohs(cliaddr.sin_port));
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        write(connfd, buf, strlen(buf));
        close(connfd);
    }
}