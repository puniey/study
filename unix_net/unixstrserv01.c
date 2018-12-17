#include "unp.h"

void str_echo(int sockfd);

int main(int argc, char **argv)
{
	int		listenfd, connfd;
	pid_t	childpid;
	socklen_t	clilen;
	struct sockaddr_un	cliaddr, servaddr;

    if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(0);
	}

	unlink(UNIXSTR_PATH);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family	= AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
	{
		perror("bind error");
		exit(0);
	}

	listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_child);

	while (1)
	{
		clilen	= sizeof(cliaddr);
		memset(&cliaddr, 0, sizeof(cliaddr));
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
		printf("accept one connect from %s, connfd:%d\n", cliaddr.sun_path, connfd);
		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}

void str_echo(int sockfd)
{
	char buf[MAXLINE];
	int	n;

	while ((n = read(sockfd, buf, MAXLINE)) > 0)
	{
		write(sockfd, buf, n);
	}
}
