//限时connect 只适用于单进程/线程程序
#include "unp.h"

static void connect_alarm(int);

int connect_timeo(int sockfd, const sockaddr *addr, socklen_t len, int nsec)
{
	Sigfunc *sigfunc;
	int		n;

	sigfunc = Signal(SIGALRM, connect_alarm);
	if (alarm(nsec) != 0)
	{
		printf("connect_timeo:alarm was already set\n");
	}

	if ((n = connect(sockfd, addr, len)) < 0)
	{
		close(sockfd);
		if (errno == EINTR)
			errno = ETIMEOUT;
	}
	alarm(0);
	Signal(SIGALRM, sigfunc);

	return n;
}

static void connect_alarm(int signo)
{
	return;
}
