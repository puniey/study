#include "unp.h"

void str_cli(int);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("usage:tcpclipoll01 <IP Address> <PORT>\n");
        return -1;
    }
    
    int sockfd;
    struct sockaddr_in servaddr;
       
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        printf("socket error, errno: %d\n", errno);
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error, errno:%d\n", errno);
        return -1;
    }
    
    str_cli(sockfd);
    
    return 0;
}

void str_cli(int sockfd)
{
    char buf[1024];
    pollfd fd[2];
    int n;
    
    n            = 0;
    fd[0].fd     = STDIN_FILENO;
    fd[0].events = POLLIN;
    fd[1].fd     = sockfd;
    fd[1].events = POLLIN;
    
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        poll(fd, 2, -1);
        
        if (fd[0].revents & POLLIN)
        {
            if ((n = read(fd[0].fd, buf, 1024)) < 0)
            {
                return;
            }
            else if (n == 0)
            {
                shutdown(fd[0].fd, SHUT_WR);
                fd[0].fd = -1;
                continue;
            }
            else
            {
                write(fd[0].fd, buf, n);
            }
        }
        
        if (fd[1].revents & (POLLIN | POLLERR))
        {
            if ((n = read(fd[1].fd, buf, 1024)) < 0)
            {
                return;
            }
            else if (n == 0)
            {
                if (fd[0].fd != -1)
                    printf("server terminated.\n");
                
                return;
            }
            else
                write(fd[1].fd, buf, n);
        }
    }
}