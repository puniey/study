#include "unp.h"
//回射服务器程序，单进程模式，循环，客户端排队连接
void str_echo(int sockfd)
{
    ssize_t n;
    char    buf[MAXLINE];
    memset(buf, 0, sizeof(buf));
    
    while (1)
    {
        n = read(sockfd, buf, sizeof(buf));
        if (n > 0)
        {
            write(sockfd, buf, n);
			break;
        }
        else if (n < 0 && errno == EINTR)
        {
            continue;
        }
        else if (n < 0)
        {
            printf("error:str_echo: %d\n", errno);
            break;            
        }
        else
        {
            printf("normal exit\n");
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clilen;
    sockaddr_in servaddr, cliaddr;
    char addr[INET_ADDRSTRLEN];
    unsigned short port;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() error, errorno:%d\n", errno);
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    if ((bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        printf("bind() error, errno:%d\n", errno);
        return -1;
    }
    
    if ((listen(listenfd, LISTENQ)) < 0)
    {
        printf("listen() error, errno:%d\n", errno);
        return -1;
    }
    
    while (1)
    {
        //printf("begin accept\n");
        clilen  = sizeof(cliaddr);
        memset(&cliaddr, 0, sizeof(cliaddr));
        if ((connfd  = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
        {
            printf("accept() error, errno:%d\n", errno);
            return -1;
        }
        //printf("accept success, connfd:%d\n", connfd);
        memset(addr, 0, sizeof(addr));
        inet_ntop(AF_INET, &cliaddr.sin_addr, addr, sizeof(addr));
        port = ntohs(cliaddr.sin_port);
        //printf("client IPAddress:%s\n", addr);
        //printf("client port:%u\n", port);
        str_echo(connfd);
        close(connfd);
    }
    return 0;
}
