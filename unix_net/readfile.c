#include "unp.h"

int main(int argc, char **argv)
{
	int	n, fd;
	char	buf[MAXLINE];
    
	if (argc != 2)
	{
		printf("usage:%s <file>\n", argv[0]);
		exit(0);
	}
	memset(buf, 0, sizeof(buf));
    if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		perror("open file error");
		exit(0);
	}

	while ((n = read(fd, buf, MAXLINE)) > 0)
	{
		write(STDOUT_FILENO, buf, n);
	}

	if (n == 0)
		printf("==========read finished===========\n");
	else
		perror("read error");
	exit(0);
}
