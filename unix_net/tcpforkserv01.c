#include "unp.h"
//TCP并发服务器 每个客户一个子进程 client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0(s)		0.512(s)
//5		5000	4000	0(s)		2.664(s)
//5		10000	4000	0.004(s)	5.24(s)
//5		5000	8000	0(s)		2.672(s)
//10	5000	4000	0.(s)		4.756(s)
//10	5000	8000	0.008(s)	4.86(s)
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
	int		listenfd, connfd;
	pid_t	childpid;
	socklen_t	clilen, addrlen;
	sockaddr_in	cliaddr;

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
	Signal(SIGCHLD, sig_child);
	
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

		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}
