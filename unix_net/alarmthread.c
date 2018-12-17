//闹钟程序 多线程版本
//每个闹钟派生一个线程
#include "unp.h"

typedef struct tagAlarm{
	int		nsecond;
	char	msg[64];
}alarm_t;

void *alarm_thread(void *arg)
{
	alarm_t *alarm = (alarm_t*)arg;
	int status;

	status = pthread_detach(pthread_self());
	if (status != 0)
		err_quit(status, "pthread_detach error");
	sleep(alarm->nsecond);
	printf("\n>>>>>(%d) %s\n", alarm->nsecond, alarm->msg);
	free(alarm);
	pthread_exit((void*)0);
}

int main(int argc, char **argv)
{
	int		status;
	char	line[128];
	alarm_t	*alarm;
	pthread_t	ptid;

	while (1)
	{
		printf("Alarm> ");
		if (fgets(line, sizeof(line), stdin) == NULL)
			exit(0);
		if (strlen(line) <= 1)
			continue;
		alarm = (alarm_t*)malloc(sizeof(alarm_t));
		if (alarm == NULL)
			err_quit(errno, "Allocate alarm");

		if (sscanf(line, "%d %64[^\n]", &alarm->nsecond, alarm->msg) < 2)
		{
			fprintf(stderr, "Bad command\n");
			free(alarm);
		}
		else
		{
			status = pthread_create(&ptid, NULL, alarm_thread, alarm);
			if (status != 0)
				err_quit(status, "pthread_create error");
		}
	}
}
