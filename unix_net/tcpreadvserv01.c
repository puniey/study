//readv分散读 需自己分配内存 没有回收 内存泄漏
#include "unp.h"

void str_echo(int sockfd)
{
	struct iovec iov[IOV_MAX];
	int n, i;
	struct iovec *tmp;

	for (i = 0; i < IOV_MAX; i++)
	{
		iov[i].iov_base = calloc(MAXLINE, 1);
		iov[i].iov_len  = MAXLINE;
	}

	while ((n = readv(sockfd, iov, IOV_MAX)) > 0)
	{
		printf("readv %d bytes\n", n);
		tmp = iov;
		i = 0;
		while (i < 10)
		{
			printf("iov[%d] buffer:%s, len:%d\n", i++, tmp->iov_base, tmp->iov_len);
			memset(tmp->iov_base, 0, tmp->iov_len);
			tmp++;
		}
	}

	if (n < 0)
	{
		perror("readv error");
	}
	else
	{
		printf("readv success finished\n");
	}
	return;
}

int main(int argc, char **argv)
{
	int listenfd, connfd, flag;
	pid_t childpid;
	socklen_t clilen;
	sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9877);
    

	bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 5);
	signal(SIGCHLD, sig_child);
    

	while (1)
	{
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
        {
			if (errno == EAGAIN)
			{
				printf("EAGAIN\n");
				continue;
			}
			else
			{
				printf("accept error\n");
				return;
			}
		}
        
		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
            close(connfd);
            return 0;
		}
        
		close(connfd);
	}
	return 0;
}
