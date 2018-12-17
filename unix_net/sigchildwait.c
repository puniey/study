#include "unp.h"

void sig_child(int signo)
{
	pid_t pid;
	int stat;
    //printf("receive SIGCHLD\n");
    //Signal(SIGCHLD, sig_child);
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("%d:child %d terminated\n", getpid(), pid);
    }
    	//printf("child %d terminated\n", pid);
    //sleep(10);
    //printf("signal func return.\n");
	return;
}

void sig_abort(int signo)
{
    printf("%d:call sig_abort()\n", getpid());
}

void sig_int(int signo)
{
	pr_cpu_time();
    exit(0);
}
