//TCP UDP select ·þÎñÆ÷
#include "unp.h"

void str_echo(int sockfd);

int main(int argc, char **argv)
{
    int     listenfd, connfd, udpfd, nready, maxfdp1;
    char    buf[MAXLINE];
    pid_t   childpid;
    fd_set  rset;
    ssize_t n;
    socklen_t   len;
    const int   on = 1;
    sockaddr_in cliaddr, servaddr;
    //create tcp fd
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket listenfd error");
        exit(0);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family         = AF_INET;
    servaddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    servaddr.sin_port           = htons(SERV_PORT);
    
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("setsockopt listenfd error");
        exit(0);
    }
    
    if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind listenfd error");
        exit(0);
    }
    
    listen(listenfd, LISTENQ);
    //create udp fd
    if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket udpfd error");
        exit(0);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family         = AF_INET;
    servaddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    servaddr.sin_port           = htons(SERV_PORT);
    
    if (bind(udpfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind udpfd error");
        exit(0);
    }
    
    Signal(SIGCHLD, sig_child);
    
    FD_ZERO(&rset);
    maxfdp1 = MAX(listenfd, udpfd) + 1;
    
    while (1)
    {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                perror("select error");
                exit(0);
            }
        }
        
        if (FD_ISSET(listenfd, &rset))
        {
            len = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &len)) < 0)
            {
                perror("accept error");
                exit(0);
            }
            
            if ((childpid = fork()) == 0)
            {
                close(listenfd);
                str_echo(connfd);
                exit(1);
            }
            close(connfd);
        }
        
        if (FD_ISSET(udpfd, &rset))
        {
            len = sizeof(cliaddr);
            n   = recvfrom(udpfd, buf, sizeof(buf), 0, (sockaddr*)&cliaddr, &len);
            sendto(udpfd, buf, n, 0, (sockaddr*)&cliaddr, len);
        }
    }
}

void str_echo(int sockfd)
{
    int     nRead;
    char    buf[MAXLINE];
    
    while ((nRead = read(sockfd, buf, sizeof(buf))) > 0)
    {
        write(sockfd, buf, nRead);
    }
    
    if (nRead < 0)
    {
        perror("read error");
    }
    else
    {
        printf("finish read.\n");
    }
}