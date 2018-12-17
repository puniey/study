//闹钟程序 主线程负责读取闹钟参数 工作线程只有一个 循环遍历闹钟列表
//有缺陷 在sleep一个闹钟时 不能处理新添加的闹钟结构
#include "unp.h"

typedef struct tagAlarm{
	struct tagAlarm *link;
	int		nsecond;
	time_t	time;
	char	msg[64];
}alarm_t;

pthread_mutex_t alarm_mutex	= PTHREAD_MUTEX_INITIALIZER;
alarm_t *alarm_list = NULL;

void *alarm_thread(void *arg)
{
	int status;
	alarm_t *alarm;
	int sleep_time;
	time_t now;

	while (1)
	{
		status = pthread_mutex_lock(&alarm_mutex);
		if (status != 0)
			err_abort(status, "Lock mutex");
		alarm = alarm_list;

		if (alarm == NULL)
			sleep_time = 1;
		else
		{
			alarm_list = alarm->link;
			now = time(NULL);
			if (alarm->time <= now)
				sleep_time = 0;
			else
				sleep_time = alarm->time - now;
#ifdef DEBUG
			printf("[waiting:%d(%d)\"%s\"]\n", alarm->time, sleep_time, alarm->msg);
#endif
		}
		status = pthread_mutex_unlock(&alarm_mutex);
		if (status != 0)
			err_abort(status, "Unlock mutex");
		if (sleep_time > 0)
			sleep(sleep_time);
		else
			sched_yield();

		if (alarm != NULL)
		{
			printf("(%d) %s\n", alarm->nsecond, alarm->msg);
			free(alarm);
		}
	}
}

int main(int argc, char **argv)
{
	int		status;
	char	line[128];
	alarm_t	*alarm, **last, *next;
	pthread_t	ptid;

	status = pthread_create(&ptid, NULL, alarm_thread, NULL);
	if (status != 0)
		err_abort(status, "Create alarm thread");

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
			status = pthread_mutex_lock(&alarm_mutex);
			if (status != 0)
				err_quit(status, "Lock mutex");
			alarm->time = time(NULL) + alarm->nsecond;

			last = &alarm_list;
			next = *last;
			while (next != NULL)
			{
				if (next->time >= alarm->time)
				{
					alarm->link = next;
					*last = alarm;
					break;
				}
				last = &next->link;
				next = next->link;
			}

			if (next == NULL)
			{
				*last = alarm;
				alarm->link = NULL;
			}
#ifdef DEBUG
			printf("[list: ");
			for (next = alarm_list; next != NULL; next = next->link)
			{
				printf("%d(%d)[\"%s\"] ", next->time, next->time - time(NULL), next->msg);
			}
			printf("]\n");
#endif
			status = pthread_mutex_unlock(&alarm_mutex);
			if (status != 0)
				err_abort(status, "Unlock mutex");
		}
	}
}
