//测试非阻塞connect
//connect失败后，sockfd处于未定义状态，需要关闭sockfd，重新socket
#include "unp.h"
//连接成功建立和失败才返回
int connect_nonb(int sockfd, const sockaddr* saptr, socklen salen, int nsec)
{
	int		flags, n, error;
	socklen_t len;
	fd_set	rset, wset;
	struct timeval tval;

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if ((n = connect(sockfd, saptr, salen)) < 0)
	{
		if (errno != EINPROGRESS)
			return -1;
	}

	if (n == 0) //connect completed immediately
	{
		fcntl(sockfd, F_SETFL, flags);
		return 0;
	}
	/*Do whatever we want while the connect is taking place.*/
    FD_ZERO(&rset)
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec		= nsec;
	tval.tv_usec	= 0;

	if ((n = select(sockfd + 1, &rset, &wset, NULL,
		        nsec ? &tval : NULL)) == 0)
	{
		errno = ETIMEDOUT;
		return -1;
	}

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
	{
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
		{
			return -1;
		}
	}
	else
	{
		printf("select error:sockfd not set.\n");
		exit(0);
	}

	fcntl(sockfd, F_SETFL, flags);

	if (error)
	{
		errno = error;
		return -1;
	}
	else
		return 0;
}
int main(int argc, char **argv)
{
	int		n, sockfd, maxfdp;
	fd_set	rset, wset;
	char	buf[MAXLINE];
	sockaddr_in	servaddr;
	
	if (argc != 3)
	{
		printf("usage:%s <IP> <PORT>\n");
		exit(0);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family	= AF_INET;
	servaddr.sin_port	= htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
	{
		perror("inet_pton error");
		exit(0);
	}

	if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
		exit(0);
	}

    while (1)
	{
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(sockfd, &rset);
	FD_SET(sockfd, &wset);
	maxfdp = sockfd + 1;
	select(maxfdp, &rset, &wset, NULL, NULL);

	if (FD_ISSET(sockfd, &rset))
	{
		printf("sockfd is ready to read\n");
		memset(buf, 0, sizeof(buf));
		if ((n = read(sockfd, buf, sizeof(buf))) > 0)
		    write(STDOUT_FILENO, buf, n);
		else
			perror("read error");
	}
	if (FD_ISSET(sockfd, &wset))
		printf("sockfd is ready to write\n");
    sleep(1);
	}

	exit(0);
}
