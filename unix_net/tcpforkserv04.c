#include "unp.h"
//TCP预先派生子进程服务器程序 accept使用线程上锁保护 client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0.004(s)	0.18(s)
//5		5000	4000	0(s)	    0.976(s)
//5		10000	4000	0.008(s)	1.916(s)
//5		5000	8000	0.02(s)		1.924(s)
//10	5000	4000	0(s)    	1.968(s)
//10	5000	8000	0.004(s)	1.98(s)
#define MAXN	16384

static int nchild;
static pid_t *pids;
static int lock_fd = -1;

void sig_int(int signo);
pid_t child_make(int i, int listenfd, int addrlen);
void child_main(int i, int listenfd, int addrlen);
void str_echo(int sockfd);

int main(int argc, char **argv)
{
	int		listenfd, i;
	socklen_t	addrlen;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 2)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<host>] <port> <childnum>\n", argv[0]);
		exit(0);
	}

	nchild = atoi(argv[argc - 1]);
	pids = calloc(nchild, sizeof(pid_t));
	if (pids == NULL)
	{
		printf("calloc error\n");
		exit(0);
	}

	my_lock_init();
	for (i = 0; i < nchild; i++)
	{
	 	pids[i] = child_make(i, listenfd, addrlen);
		//printf("fork pid:%d\n", pids[i]);
	}

	Signal(SIGINT, sig_int);

	while (1)
		pause();
	exit(0);
}

void sig_int(int signo)
{
	int i;
	for (i = 0; i < nchild; i++)
	{
		kill(pids[i], SIGTERM);
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

pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;
	
	if ((pid = fork()) > 0)
		return pid;

	child_main(i, listenfd, addrlen);
}

void child_main(int i, int listenfd, int addrlen)
{
	int		connfd;
	socklen_t	clilen;
	sockaddr_in	cliaddr;

	while (1)
	{
		clilen = addrlen;
		my_lock_wait();
		//printf("begin accept\n");
		if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
		{
			perror("accept error");
			exit(0);
		}
		my_lock_release();
		//printf("accept success\n");
		str_echo(connfd);
		close(connfd);
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
		memset(buf, 1, sizeof(buf));
		buf[n] = 0;
		writen(sockfd, buf, n);
    }
}
