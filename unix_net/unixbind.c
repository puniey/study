#include "unp.h"

int main(int argc, char **argv)
{
	int		sockfd;
	socklen_t	len;
	struct sockaddr_un addr1, addr2;

	if (argc != 2)
	{
		printf("usage: %s <path>\n", argv[0]);
		exit(0);
	}

	if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(0);
	}

	memset(&addr1, 0, sizeof(addr1));
	addr1.sun_family	= AF_LOCAL;
	strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path) - 1);
	printf("addr1 size:%d\n", SUN_LEN(&addr1));
	unlink(argv[1]);
	if (bind(sockfd, (sockaddr*)&addr1, SUN_LEN(&addr1)) != 0)
	{
		perror("bind error");
		exit(0);
	}

	len = sizeof(addr2);
	printf("addr2 len:%d\n", len);
	if (getsockname(sockfd, (sockaddr*)&addr2, &len) != 0)
	{
		perror("getsockname error");
		exit(0);
	}
	printf("bound name = %s, returned len = %d\n", addr2.sun_path, len);
	exit(0);
}
