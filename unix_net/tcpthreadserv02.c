//TCP预先创建线程服务器程序，每个线程各自accept
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0(s)		0.172(s)
//5		5000	4000	0.012(s)	0.852(s)
//5		10000	4000	0.024(s)	1.656(s)
//5		5000	8000	0(s)		0.86(s)
//10	5000	4000	0.008(s)	1.72(s)
//10	5000	8000	0.000(s)	1.74(s)
#include "unp.h"

#define MAXN	12888

Thread *tptr;
int		listenfd, nthreads;
socklen_t	addrlen;
pthread_mutex_t mLock = PTHREAD_MUTEX_INITIALIZER;

void sig_int(int);
void thread_make(int);
void *thread_main(void*);
void str_echo(int);

int main(int argc, char **argv)
{
	int		i;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<host>] <port> <threads>\n", argv[0]);
		exit(0);
	}

	nthreads = atoi(argv[argc - 1]);
	tptr	 = calloc(nthreads, sizeof(Thread));

	if (tptr == NULL)
		err_quit(errno, "calloc error");

	for (i = 0; i < nthreads; i++)
	{
		thread_make(i);
	}
	
	Signal(SIGINT, sig_int);
	while (1)
		pause();
}

void  thread_make(int i)
{
	int status = pthread_create(&tptr[i].ptid, NULL, thread_main, (void*)i);
	if (status != 0)
		err_quit(status, "pthread_create error");
	return;
}

void *thread_main(void *arg)
{
	int		connfd;
	socklen_t clilen;
	struct sockaddr *cliaddr;
	int		i = (int)arg;

	cliaddr = malloc(addrlen);

	while (1)
	{
		clilen = addrlen;
		pthread_mutex_lock(&mLock);
		connfd = accept(listenfd, cliaddr, &clilen);
		pthread_mutex_unlock(&mLock);
		tptr[i].tCount++;
		str_echo(connfd);
		close(connfd);
	}
	pthread_exit((void*)0);
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
	for (i = 0; i < nthreads; i++)
	{
		printf("thread %d:%d\n", i, tptr[i].tCount);
	}
	pr_cpu_time();
	exit(0);
}
