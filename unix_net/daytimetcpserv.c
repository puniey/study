#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	char buff[255];
    struct sockaddr_in seraddr;
	time_t ticks;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		return 0;
	}

	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_port   = htons(13);
	bind(listenfd, (struct sockaddr*)&seraddr, sizeof(struct sockaddr));
	listen(listenfd, 5);
	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		printf("%d\n", connfd);
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		write(connfd, buff, strlen(buff));
		close(connfd);
	}
	return 0;
}
