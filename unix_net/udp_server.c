#include "unp.h"

int udp_server(const char *host, const char *serv, socklen_t *lenptr)
{
    int		sockfd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags	= AI_PASSIVE;
	hints.ai_family	= AF_INET;
	hints.ai_socktype	= SOCK_DGRAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("udp_server getaddrinfo error for %s, %s:%s\n",
		    host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;

	while (res != NULL)
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;
		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(sockfd);
		res = res->ai_next;
	}

	if (res == NULL)
	{
		printf("udp_server error for %s %s\n", host, serv);
		return -1;
	}
	else
	{
		char buf[125];
		if (lenptr)
			*lenptr = res->ai_addrlen;
		printf("udp_server bind sucess for %s %s[%s:%d]\n",
		    host, serv, inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, buf, sizeof(buf)), ntohs(((sockaddr_in*)res->ai_addr)->sin_port));
		freeaddrinfo(ressave);
		return sockfd;
	}
}
