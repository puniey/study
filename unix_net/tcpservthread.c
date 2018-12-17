#include "unp.h"

static void *doit(void *);
void str_echo(int sockfd);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pthread_t tid;
    socklen_t addrlen;
    sockaddr_in cliaddr, servaddr;
    int value = 1;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9877);
    
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
	bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 5);
    
    while (1)
    {
        addrlen = sizeof(cliaddr);
        connfd  = accept(listenfd, (sockaddr*)&cliaddr, &addrlen);
        pthread_create(&tid, NULL, &doit, (void*)connfd);
    }
    
    return 0;
}

static void *doit(void *arg)
{
    pthread_detach(pthread_self());
    str_echo((int)arg);
    close((int)arg);
    return NULL;
}

void str_echo(int sockfd)
{
    int nRead;
    char recvbuf[MAXLINE];
    memset(recvbuf, 0, sizeof(recvbuf));
    
    while (1)
    {
        if ((nRead = read(sockfd, recvbuf, MAXLINE)) <= 0)
            return;
        else
            write(sockfd, recvbuf, nRead);
    }
    return;
}