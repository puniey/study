//tcp 并发服务器程序 每个客户一个线程 client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0(s)		0.256(s)
//5		5000	4000	0(s)		1.160(s)
//5		10000	4000	0.012(s)	2.516(s)
//5		5000	8000	0(s)		1.248(s)
//10	5000	4000	0.004(s)	2.992(s)
//10	5000	8000	0.004(s)	3.212(s)
#include "unp.h"
#define MAXN 12888
void str_echo(int sockfd);
void sig_int(int);
void *doit(void *);

int main(int argc, char **argv)
{
	int		listenfd, connfd;
	pthread_t	ptid;
	socklen_t	clilen, addrlen;
	struct sockaddr *cliaddr;
	int status;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<host>] <port>");
		exit(0);
	}

	Signal(SIGINT, sig_int);

	while (1)
	{
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) < 0)
			err_quit(errno, "accept error");
		status = pthread_create(&ptid, NULL, doit, (void*)connfd);
		if (status != 0)
			err_quit(status, "pthread_create error");
	}
}

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
		memset(buf, '1', sizeof(buf));
		buf[n] = 0;
		writen(sockfd, buf, n);
    }
}

void sig_int(int signo)
{
	pr_cpu_time();
	exit(0);
}

void *doit(void *arg)
{
	pthread_detach(pthread_self());
	int connfd = (int)arg;
	str_echo(connfd);
	close(connfd);
	pthread_exit((void*)0);
}
