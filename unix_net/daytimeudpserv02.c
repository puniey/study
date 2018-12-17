#include "unp.h"

int main(int argc, char **argv)
{
	int		sockfd;
	ssize_t	n;
	char	buf[MAXLINE];
	time_t	ticks;
	socklen_t	len;
	sockaddr_in	cliaddr;

	if (argc == 2)
	{
		sockfd = udp_server(NULL, argv[1], NULL);
	}
	else if (argc == 3)
	{
		sockfd = udp_server(argv[1], argv[2], NULL);
	}
	else
	{
		printf("usage:%s [<host>] <service>\n", argv[0]);
		exit(0);
	}

	while (1)
	{
		len = sizeof(cliaddr);
		n	= recvfrom(sockfd, buf, MAXLINE, 0, (sockaddr*)&cliaddr, &len);
		printf("datagram from %s:%d\n", 
		    inet_ntop(AF_INET, &cliaddr.sin_addr, buf, MAXLINE), ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
		sendto(sockfd, buf, strlen(buf), 0, (sockaddr*)&cliaddr, len);
	}
}
