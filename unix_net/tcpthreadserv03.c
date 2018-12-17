//TCP预先创建线程服务器程序 主线程统一accept client.c
//client发起进程	单进程TCP连接	请求bytes	serv用户时间	serv系统时间
//1		5000	4000	0(s)		0.176(s)
//5		5000	4000	0.012(s)	0.872(s)
//5		10000	4000	0.016(s)	1.716(s)
//5		5000	8000	0.008(s)	0.848(s)
//10	5000	4000	0.008(s)	1.68(s)
//10	5000	8000	0.008(s)	1.744(s)
#include "unp.h"

Thread *tptr;
#define MAXCLI	32
#define MAXN	12888
int	clifd[MAXCLI], iget, iput;
pthread_mutex_t	clifd_mutex	= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	clifd_cond	= PTHREAD_COND_INITIALIZER;
static int nthreads;

void sig_int(int);
void thread_make(int);
void *thread_main(void*);
void str_echo(int);

int main(int argc, char **argv)
{
	int		i, listenfd, connfd;
	socklen_t addrlen, clilen;
	struct sockaddr *cliaddr;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
	{
		printf("usage:%s [<host>] <port> <threads>\n", argv[0]);
		exit(0);
	}

	cliaddr  = malloc(addrlen);
	nthreads = atoi(argv[argc - 1]);
	tptr	 = calloc(nthreads, sizeof(Thread));
	iget = iput = 0;

	if (tptr == NULL)
		err_quit(errno, "calloc error");

	for (i = 0; i < nthreads; i++)
	{
		thread_make(i);
	}
	
	Signal(SIGINT, sig_int);
	while (1)
    {
		clilen = addrlen;
		connfd = accept(listenfd, cliaddr, &clilen);

		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		if (++iput == MAXCLI)
			iput = 0;
		if (iput == iget)
			err_quit(0, "iput == iget");
		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	}
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
	int		i = (int)arg;

	while (1)
	{
		pthread_mutex_lock(&clifd_mutex);
		while (iget == iput)
			pthread_cond_wait(&clifd_cond, &clifd_mutex);
		connfd = clifd[iget];
		if (++iget == MAXCLI)
			iget = 0;
		pthread_mutex_unlock(&clifd_mutex);
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
