//socketpair 实现父子进程通信
//子进程打开文件 传递描述符至父进程读写
//具有亲缘关系的描述符传递的实例
//相关文件 openfile.c
#include "unp.h"

int my_open(const char *, int);

int main(int argc, char **argv)
{
	int		fd, n;
	char	buf[MAXLINE];

	if (argc != 2)
	{
		printf("usage:%s <pathname>\n", argv[0]);
		exit(0);
	}

	if ((fd = my_open(argv[1], O_RDONLY)) < 0)
	{
		perror("cannot open file");
		exit(0);
	}
	while ((n = read(fd, buf, MAXLINE)) > 0)
	{
		write(STDOUT_FILENO, buf, n);
	}
	exit(0);
}

int my_open(const char *pathname, int mode)
{
	int		fd, sockfd[2], status;
	pid_t	childpid;
	char    c, argsockfd[10], argmode[10];

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) < 0)
	{
		perror("socketpair error");
		return -1;
	}

	if ((childpid = fork()) == 0)
	{
		close(sockfd[0]);
		snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
		snprintf(argmode, sizeof(argmode), "%d", mode);
		execl("./openfile", "openfile", argsockfd, pathname, argmode, 
		    (char*)NULL);
		printf("execl error\n");
		exit(0);
	}

	close(sockfd[1]);
	waitpid(childpid, &status, 0);
	if (WIFEXITED(status) == 0)
	{
		printf("child did not terminate\n");
		exit(0);
	}
	if ((status = WEXITSTATUS(status)) == 0)
	{
		if (read_fd(sockfd[0], &c, 1, &fd) <= 0)
        {
			perror("read_fd error");
		}
	}
	else
	{
		errno = status;
		fd = -1;
	}

	close(sockfd[0]);
	return fd;
}
