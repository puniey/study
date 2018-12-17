#include "unp.h"
//回射服务器程序，多进程模式，循环
//apachebench 测试
//  请求  并发  每秒请求数
//  10000 10    2379.81/s
//  10000 20    2306.78/s
// 100000  1    2516.84/s
// 100000 10    2403.30/s
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
            //abort();
            //close(sockfd);
            //return;
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
            //printf("normal exit\n");
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
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
    Signal(SIGCHLD, sig_child);
    Signal(SIGABRT, sig_abort);
    while (1)
    {
        printf("begin accept, this is father pid:%d\n", getpid());
        clilen  = sizeof(cliaddr);
        memset(&cliaddr, 0, sizeof(cliaddr));
		sleep(60);
        if ((connfd  = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
        {
            if (errno == EINTR)
                continue;
            printf("accept() error, errno:%d\n", errno);
            return -1;
        }
        childpid = fork();
        if (childpid == 0)
        {
            close(listenfd);
            //printf("accept success, connfd:%d, this is child pid:%d\n", connfd, getpid());
            memset(addr, 0, sizeof(addr));
            inet_ntop(AF_INET, &cliaddr.sin_addr, addr, sizeof(addr));
            port = ntohs(cliaddr.sin_port);
            //printf("client IPAddress:%s\n", addr);
            //printf("client port:%u\n", port);
            str_echo(connfd);
            //exit(0);
            //return 0;
            //abort();
        }
        else if (childpid < 0)
        {
            printf("fork() error:%d\n", childpid);
        }
        close(connfd);
        int ret = sleep(10);
        printf("sleep return %d\n", ret);
        //waitpid(-1, NULL, WNOHANG);
    }
    return 0;
}
