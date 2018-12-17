#include "unp.h"
//apachebench 测试
//  请求  并发  每秒请求数
//  10000 10    13890.99/s
//  10000 20    13264.55/s
// 100000  1    13556.02/s
// 100000 10    14124.57/s
int main(int argc, char** argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nReady, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t clilen;
    sockaddr_in cliaddr, servaddr;
    int value = 1;
    int nSendBuf = 0, nRecvBuf = 0, nLen;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() error, errorno:%d", errno);
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(9877);
    
    //setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    
    if ((bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        printf("bind() error, errno:%d\n", errno);
        return -1;
    }
    nLen = sizeof(nSendBuf);
    getsockopt(listenfd, SOL_SOCKET, SO_SNDLOWAT, &nSendBuf, &nLen);
    nLen = sizeof(nRecvBuf);
    nRecvBuf = 10;
    setsockopt(listenfd, SOL_SOCKET, SO_RCVLOWAT, &nRecvBuf, nLen);
    getsockopt(listenfd, SOL_SOCKET, SO_RCVLOWAT, &nRecvBuf, &nLen);
    printf("send buf lowat size:%d\n", nSendBuf);
    printf("receive buf lowat size:%d\n", nRecvBuf);
    
    if((listen(listenfd, 10)) < 0)
    {
        printf("listen() error, errno:%d\n", errno);
        return -1;
    }
    
    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; ++i)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    
    while(1)
    {
        rset   = allset;
        nReady = select(maxfd + 1, &rset, NULL, NULL, NULL);
        //printf("%d sockets ready to read.\n", nReady);
        if (FD_ISSET(listenfd, &rset))
        {
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
            {
                printf("accept() error, errno:%d\n", errno);
                continue;
            }
            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }
            
            if (i >= FD_SETSIZE)
            {
                printf("too many clients\n");
                return -1;
            }
            
            FD_SET(connfd,&allset);
            if (connfd > maxfd)
                maxfd = connfd;
            
            if (i > maxi)
                maxi = i;
            
            if (--nReady <= 0)
                continue;
        }
        
        for (i = 0; i <= maxi; ++i)
        {
            if ((sockfd = client[i]) < 0)
                continue;
            
            if (FD_ISSET(sockfd, &rset))
            {
                //printf("sockfd:%d\n", sockfd);
                memset(buf, 0, sizeof(buf));
                if ((n = read(sockfd, buf, sizeof(buf))) == 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    printf("client closed, fd:%d\n", sockfd);
                }
                else
                {
                    //printf("tcpserv write: %d bytes buf: %s\n", n, buf);
                    write(sockfd, buf, n);
                    //printf("server close socket:%d\n", sockfd);
                    /*
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    */
                }
                
                if (--nReady <= 0)
                    break;
            }
        }
    }
}