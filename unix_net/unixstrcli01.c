#include "unp.h"

void str_cli(int);

int main(int argc, char **argv)
{
	int		sockfd;
	struct sockaddr_un servaddr, cliaddr;

	if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(0);
	}
    printf("sockfd:%d\n");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family	= AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
		exit(0);
	}

	str_cli(sockfd);
	exit(0);
}

void str_cli(int sockfd)
{
	char	buf[MAXLINE];
	int		nRead, nWrite;

	while ((nRead = read(STDIN_FILENO, buf, MAXLINE)) > 0)
	{
		write(sockfd, buf, nRead);
		nWrite = read(sockfd, buf, MAXLINE);
		write(STDOUT_FILENO, buf, nWrite);
	}
}
