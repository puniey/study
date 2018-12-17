#include "unp.h"
//回射服务器程序，单进程模式，循环，客户端排队连接
//TCP迭代服务器 测试服务器性能 作为基准 客户端client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0(s)		0.164(s)
//5		5000	4000	0(s)		0.656(s)
//5		10000	4000	0.016(s)	1.296(s)
//5		5000	8000	0(s)		0.668(s)
//10	5000	4000	0.016(s)	1.134(s)
//10	5000	8000	0(s)		1.32(s)
#define MAXN	16384

void str_echo(int sockfd)
{
    ssize_t n;
    char    buf[MAXN];
    memset(buf, 0, sizeof(buf));
    
    while (1)
    {
        if (read(sockfd, buf, sizeof(buf)) == 0)
			break;
		n = atoi(buf);
		if (n <= 0 || n > MAXN)
		{
			printf("error client request for %d bytes\n", n);
			break;
		}
		memset(buf, 1, sizeof(buf));
		buf[n] = 0;
		writen(sockfd, buf, n);
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clilen, addrlen;
    sockaddr_in cliaddr;
    char addr[INET_ADDRSTRLEN];

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<IP>] <port>\n", argv[0]);
		exit(0);
	}

    Signal(SIGINT, sig_int);
	
	while (1)
	{
		clilen = addrlen;
		if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("accept error");
				exit(0);
			}
		}
		str_echo(connfd);
		close(connfd);
	}
	exit(0);
}
