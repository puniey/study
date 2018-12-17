#include "unp.h"

Sigfunc* Signal(int signo, Sigfunc* func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
    //printf("signal func\n");
	if (signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
        printf("signal-1\n");
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else
	{
#ifdef SA_RESTART
        //printf("signal-restart\n");
		act.sa_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}

void pr_mask(const char *str)
{
    sigset_t    sigset;
    int         errno_save;
    errno_save  = errno;
    
    if (sigprocmask(0, NULL, &sigset) < 0)
    {
        printf("sigprocmask error\n");
        exit(0);
    }
    
    printf("%s mask signal:", str);
    if (sigismember(&sigset, SIGINT))
        printf("SIGINT ");
    if (sigismember(&sigset, SIGQUIT))
        printf("SIGQUIT ");
    if (sigismember(&sigset, SIGUSR1))
        printf("SIGUSR1 ");
    if (sigismember(&sigset, SIGUSR2))
        printf("SIGUSR2 ");
    if (sigismember(&sigset, SIGALRM))
        printf("SIGALRM ");
    printf("\n");
    errno = errno_save;
    return;
}

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr(int signo)
{
    sigflag = 1;
}

void TELL_WAIT(void)
{
    if (Signal(SIGUSR1, sig_usr) < 0)
    {
        printf("signal SIGUSR1 error\n");
        exit(0);
    }
    if (Signal(SIGUSR2, sig_usr) < 0)
    {
        printf("signal SIGUSR2 error\n");
        exit(0);
    }
    
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        printf("sigpromask block error\n");
        exit(0);
    }
}

void TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

void WAIT_PARENT(void)
{
    while (sigflag == 0)
    {
        sigsuspend(&zeromask);
    }
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("sigprocmask unblock error\n");
        exit(0);
    }
}

void TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
}

void WAIT_CHILD(void)
{
    while (sigflag == 0)
    {
        sigsuspend(&zeromask);
    }
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("sigprocmask unblock error\n");
        exit(0);
    }
}
