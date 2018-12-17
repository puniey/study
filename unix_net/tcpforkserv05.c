//TCP预先派生子进程服务器程序，传递描述符 client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0.008(s)	0.248(s)
//5		5000	4000	0.004(s)	1.084(s)
//5		10000	4000	0.000(s)	2.156(s)
//5		5000	8000	0.00(s)		1.128(s)
//10	5000	4000	0(s)    	2.192(s)
//10	5000	8000	0.016(s)	2.168(s)
#include "unp.h"

#define MAXN	16384

Child *cptr;
static int nchildren;

pid_t child_make(int i, int listenfd, int addrlen);
void child_main(int, int, int);
void sig_int(int);
void str_echo(int sockfd);

int main(int argc, char **argv)
{
	int		listenfd, i, navail, maxfd, nsel, connfd, rc;
	ssize_t	n;
	fd_set	rset, masterset;
	socklen_t	addrlen, clilen;
	struct sockaddr *cliaddr;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<host>] <port> <#children>\n", argv[0]);
		exit(0);
	}

	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd;
	cliaddr = malloc(addrlen);

	nchildren = atoi(argv[argc - 1]);
	navail	  = nchildren;
	cptr	  = calloc(nchildren, sizeof(Child));

	for (i = 0; i < nchildren; i++)
	{
		child_make(i, listenfd, addrlen);
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = MAX(maxfd, cptr[i].child_pipefd);
	}

	Signal(SIGINT, sig_int);

	while (1)
	{
		rset = masterset;
		if (navail <= 0)
			FD_CLR(listenfd, &rset);
		nsel = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset))
		{
			clilen = addrlen;
			connfd = accept(listenfd, cliaddr, &clilen);

			for (i = 0; i < nchildren; i++)
			{
				if (cptr[i].child_status == 0)
					break;
			}

			if (i == nchildren)
			{
				printf("no available children\n");
				exit(0);
			}

			cptr[i].child_status = 1;
			cptr[i].child_count++;
			navail--;

			n = write_fd(cptr[i].child_pipefd, " ", 1, connfd);
			close(connfd);
			if (--nsel == 0)
				continue;
		}

		for (i = 0; i < nchildren; i++)
		{
			if (FD_ISSET(cptr[i].child_pipefd, &rset))
			{
				if ((n = read(cptr[i].child_pipefd, &rc, 1)) == 0)
				{
					printf("child %d terminated unexpectedly\n", i);
					exit(0);
				}
				cptr[i].child_status = 0;
				navail++;
				if (--nsel == 0)
					break;
			}
		}
	}
}

pid_t child_make(int i, int listenfd, int addrlen)
{
	int		sockfd[2];
	pid_t	pid;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) < 0)
		err_quit(errno, "socketpair error");

	pid = fork();
	if (pid < 0)
		err_quit(errno, "fork error");
	else if (pid > 0)
	{
		close(sockfd[1]);
		cptr[i].child_pid	= pid;
		cptr[i].child_pipefd= sockfd[0];
		cptr[i].child_status= 0;
		return pid; //parent process
	}

	dup2(sockfd[1], STDERR_FILENO);
	close(sockfd[0]);
	close(sockfd[1]);
	close(listenfd);
	child_main(i, listenfd, addrlen);
}

void child_main(int i, int listenfd, int addrlen)
{
	char	c;
	int		connfd;
	ssize_t	n;

	while (1)
	{
		if ((n = read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0)
			err_quit(errno, "read_Fd error");
		if (connfd < 0)
			err_quit(0, "no descriptor from read_fd");
		str_echo(connfd);
		close(connfd);

		write(STDERR_FILENO, " ", 1);
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
	int i;
	for (i = 0; i < nchildren; i++)
	{
		printf("child %d:%d\n", i, cptr[i].child_count);
		kill(cptr[i].child_pid, SIGTERM);
	}

	while (wait(NULL) > 0);

	if (errno != ECHILD)
	{
		printf("wait error\n");
		exit(0);
	}
	pr_cpu_time();
	exit(0);
}
