//测试SO_LINGER
#include "unp.h"

int main(int argc, char **argv)
{
	int		sockfd;
	struct linger ling;
	struct sockaddr_in servaddr;

	if (argc != 3)
	{
		printf("usage:%s <IP> <PORT>\n", argv[0]);
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
    //	/*
    ling.l_onoff = 1;
	ling.l_linger= 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0)
	{
		perror("setsockopt error");
		exit(0);
	}
    //*/
	close(sockfd);
	exit(0);
}
