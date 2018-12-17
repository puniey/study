#include "unp.h"

int main(int argc, char **argv)
{
	int		fd, n;

	if (argc != 4)
	{
		printf("usage:%s <sockfd> <filename> <mode>\n", argv[0]);
		exit(255);
	}

	if ((fd = open(argv[2], atoi(argv[3]))) < 0)
	{
		exit(errno);
	}
	if ((n = write_fd(atoi(argv[1]), "1", 1, fd)) < 0)
	{
		perror("write_fd error");
		exit(errno);
	}
	exit(0);
}
