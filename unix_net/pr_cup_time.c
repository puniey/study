//打印耗费在执行用户进程上的CPU时间和内核在调用进程执行系统调用上耗费CPU的时间
#include "unp.h"

void pr_cpu_time(void)
{
	double	user, sys;
	struct rusage	myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0)
	{
		perror("getrusage error");
		exit(0);
	}
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0)
	{
		perror("getrusage error");
		exit(0);
	}

	user = (double)myusage.ru_utime.tv_sec + 
		myusage.ru_utime.tv_usec / 1000000.0;
	user += (double)childusage.ru_utime.tv_sec +
		childusage.ru_utime.tv_usec / 1000000.0;
	sys = (double)myusage.ru_stime.tv_sec +
		myusage.ru_stime.tv_usec / 1000000.0;
	sys += (double)childusage.ru_stime.tv_sec +
		childusage.ru_stime.tv_usec / 1000000.0;

	printf("\nuser time = %f, sys_time = %f\n", user, sys);
}
