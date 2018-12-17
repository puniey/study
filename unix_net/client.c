//TCP测试用客户程序
//用于测试各类服务器变体的性能
#include "unp.h"

#define MAXN	16384

int main(int argc, char **argv)
{
	int		i, j, fd, childnum, nloop, nbytes;
	pid_t	pid;
	ssize_t	n;
	char	request[MAXLINE], reply[MAXN];
	struct timeval tval;
	double begin, end;

	if (argc != 6)
	{
		printf("usage:%s <IP> <port> <childnum> <nloop> <requesbytes>\n", argv[0]);
		exit(0);
	}

	childnum	= atoi(argv[3]);
	nloop		= atoi(argv[4]);
    nbytes		= atoi(argv[5]);
	snprintf(request, sizeof(request), "%d", nbytes);
	gettimeofday(&tval, NULL);
	begin = (double)tval.tv_sec + tval.tv_usec / 1000000.0;
	for (i = 0; i < childnum; i++)
	{
		if ((pid = fork()) == 0)
		{
			for (j = 0; j < nloop; j++)
			{
				if ((fd = tcp_connect(argv[1], argv[2])) < 0)
					exit(0);
				//while (1)
				{
					write(fd, request, strlen(request));
					if ((n = readn(fd, reply, nbytes)) != nbytes)
					{
						printf("error:server returned %d bytes\n", n);
						exit(0);
					}
				}
				close(fd);
			}
	        //printf("child %d done\n", i);
			exit(0);
		}
	}

	while ((pid = wait(NULL)) > 0);

	if (errno != ECHILD)
		perror("wait error");
	gettimeofday(&tval, NULL);
	end = (double)tval.tv_sec + tval.tv_usec / 1000000.0;
	printf("client finish :%g(sec)\n", (end - begin));
	exit(0);
}
