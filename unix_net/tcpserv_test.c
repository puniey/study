#include "unp.h"

int main(int argc, char** argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nReady, client[MAX_CLI];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t clilen;
    sockaddr_in cliaddr, servaddr;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() error, errorno:%d", errno);
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(9878);
    
    if ((bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        printf("bind() error, errno:%d\n", errno);
        return -1;
    }
    
    if((listen(listenfd, 10)) < 0)
    {
        printf("listen() error, errno:%d\n", errno);
        return -1;
    }
    
    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < MAX_CLI; ++i)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    
    while(1)
    {
        rset   = allset;
        nReady = select(maxfd + 1, &rset, NULL, NULL, NULL);
        printf("%d sockets ready to read\n", nReady);
        if (FD_ISSET(listenfd, &rset))
        {
			sleep(5);
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
            {
				perror("accpet error");
                continue;
            }
            printf("sockfd:%d connected\n", connfd);
            for (i = 0; i < MAX_CLI; ++i)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }
            
            if (i >= MAX_CLI)
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
                if ((n = read(sockfd, buf, sizeof(buf))) == 0)
                {
                    printf("sockfd:%d closed\n", sockfd);
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                else if (n < 0)
                {
					perror("read error");
                    return -1;
                }
                else
                {
                    char msg[255];
                    memset(msg, 0, sizeof(msg));
                    sprintf(msg, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n%s",2, "ok");
                    write(sockfd, msg, strlen(msg));
                    //write(STDOUT_FILENO, buf, n);
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                
                if (--nReady <= 0)
                    break;
            }
        }
    }
}
