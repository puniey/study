#include "unp.h"
//apachebench 测试
//ab -n 10000 -c 10 127.0.0.1:9877/
//  请求  并发  每秒请求数
//  10000 10    13519.49/s
//  10000 20    13423.20/s
// 100000  1    13727.81/s
// 100000 10    14685.73/s
int main(int argc, char** argv)
{
    int i, maxi, listenfd, connfd, sockfd;
    int nReady;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    pollfd client[OPEN_MAX];
    sockaddr_in cliaddr, servaddr;
    int value = 1;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error, errno:%d\n", errno);
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(9877);
    
    //setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    
    if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error, errno:%d\n", errno);
        return -1;
    }
    if (listen(listenfd, LISTENQ) < 0)
    {
        printf("listen error, errno:%d\n", errno);
        return -1;
    }
    
    client[0].fd     = listenfd;
    client[0].events = POLLIN;
    
    for (i = 1; i < OPEN_MAX; ++i)
    {
        client[i].fd = -1;
    }
    
    maxi = 0;
    
    while(1)
    {
        nReady = poll(client, maxi + 1, INFTIM);
        if (client[0].revents & POLLIN)
        {
            clilen = sizeof(cliaddr);
            if ((connfd = accept(client[0].fd, (sockaddr*)&cliaddr, &clilen)) < 0)
            {
                printf("accept error, errno:%d\n", errno);
                continue;
            }
            
            for (i = 1; i < MAX_CLI; ++i)
            {
                if (client[i].fd < 0)
                {
                    client[i].fd     = connfd;
                    client[i].events = POLLIN;
                    break;
                }
            }
            
            if (i == MAX_CLI)
            {
                printf("too many clients\n");
                return -1;
            }
            
            if (i > maxi)
                maxi = i;
            
            if (--nReady <= 0)
                continue;
        }
        
        for (i = 1; i <= maxi; ++i)
        {
            if ((sockfd = client[i].fd) < 0)
                continue;
            
            if (client[i].revents & (POLLIN | POLLERR))
            {
                if ((n = read(sockfd, buf, MAXLINE)) <= 0)
                {
                    if (errno == ECONNRESET || n == 0)
                    {
                        close(sockfd);
                        client[i].fd = -1;
                        printf("client closed, fd:%d\n", sockfd);
                    }
                    else
                    {
                        printf("read error:%d\n", errno);
                        return -1;
                    }
                }
                else
                {
                    //write(sockfd, buf, n);
                    /*
                    char msg[MAXLINE * 10];
                    char str[MAXLINE * 10];
                    
                    memset(msg, 0, sizeof(msg));
                    memset(str, 0, sizeof(str));
                    for (i = 0; i < 2; ++i)
                    {
                        strcat(str, "hello");
                    }
                    sprintf(msg, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n%s", strlen(str), str);
                    write(sockfd, msg, strlen(msg) + 1);*/
                    close(sockfd);
                    client[i].fd = -1;
                }
                
                if (--nReady <= 0)
                    break;
            }
        }
    }
}
