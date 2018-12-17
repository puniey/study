//闹钟程序 简版
#include "unp.h"

int main(int argc, char **argv)
{
	int		nseconds;
	char	line[128];
	char	msg[64];

	while(1)
	{
		printf("Alarm> ");
		if (fgets(line, sizeof(line), stdin) == NULL)
			exit(0);
		if (strlen(line) <= 1)
			continue;
		if (sscanf(line, "%d %64[^\n]", &nseconds, msg) < 2)
			fprintf(stderr, "Bad command\n");
		else
		{
			sleep(nseconds);
			printf("(%d) %s\n", nseconds, msg);
		}
	}
}
