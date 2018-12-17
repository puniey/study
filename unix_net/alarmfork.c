//闹钟程序 多进程版
#include "unp.h"

int main(int argc, char **argv)
{
	char	line[128];
	int		nsec;
	pid_t	pid;
	char	msg[64];

	Signal(SIGCHLD, sig_child);

	while (1)
	{
		printf("Alarm> ");
		if (fgets(line, sizeof(line), stdin) == NULL)
			exit(0);
		if (strlen(line) <= 1)
			continue;
		if (sscanf(line, "%d %64[^\n]", &nsec, msg) < 2)
			fprintf(stderr, "Bad command\n");
		else
		{
			pid = fork();
			if (pid == -1)
				err_quit(errno, "fork error");
			else if (pid == 0)
			{
				sleep(nsec);
				printf(">>>>time up:(%d) %s\n", nsec, msg);
				exit(0);
			}
		}
	}
}
