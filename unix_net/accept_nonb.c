//测试非阻塞accept
#include "unp.h"

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
    int listenfd, connfd, flags;
    socklen_t clilen;
    sockaddr_in servaddr, cliaddr;
    char addr[INET_ADDRSTRLEN];
    unsigned short port;
	fd_set rset, wset;
	char buf[MAXLINE];
	int n, maxfdp;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() error, errorno:%d\n", errno);
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
	
    flags = fcntl(listenfd, F_GETFL, 0);
	fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);
    
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

	FD_ZERO(&rset);
	FD_SET(listenfd, &rset);

    if (select(listenfd + 1, &rset, NULL, NULL, 0) < 0)
	{
		perror("select error");
		exit(0);
	}

	if (FD_ISSET(listenfd, &rset))
	{
		printf("listenfd readable\n");
		sleep(10);
		if ((connfd = accept(listenfd, NULL, NULL)) < 0)
		{
			perror("listen error");
			exit(0);
		}
		printf("accept success:%d\n", connfd);
		close(connfd);
	}
	close(listenfd);
    return 0;
}
