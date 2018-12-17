#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[255];
    sockaddr_in seraddr;

	if (argc != 2)
	{
		printf("lack of parameter\n");
		return 0;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		return 0;
	}

	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port   = htons(13);
		
	if (inet_pton(AF_INET, argv[1], &seraddr.sin_addr) <= 0)
	{
		printf("inet_pton error\n");
		return 0;
	}

	if (connect(sockfd, (sockaddr*)&seraddr, sizeof(seraddr)) < 0)
	{
		perror("connect error");
		return 0;
	}

	while ((n = read(sockfd, recvline, 255)) > 0)
	{
		recvline[n] = 0;
		/*if (fputs(recvline, stdout) == EOF)
		{
			printf("fputs error");
			return 0;
		}*/
		printf("%s", recvline);
	}

	if (n < 0)
	{
		printf("read error\n");
		return 0;
	}

	return 0;
}
