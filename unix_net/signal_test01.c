#include "unp.h"

static void sig_usr(int signo)
{
    if (signo == SIGUSR1)
        printf("pid %d receive signal SIGUSR1\n", getpid());
    else if (signo == SIGUSR2)
        printf("pid %d receive signal SIGUSR2\n", getpid());
    else if (signo == SIGCHLD)
        printf("pid %d receive signal SIGCHLD\n", getpid());
    else if (signo == SIGALRM)
        printf("pid %d receive signal SIGALRM\n", getpid());
    else
        printf("pid %d receive signal:%d\n", getpid(), signo);
}

int main(int argc, char **argv)
{
    if (Signal(SIGUSR1, sig_usr) == SIG_ERR)
    {
        printf("can not catch SIGUSR1\n");
        exit(0);
    }
    if (Signal(SIGUSR2, sig_usr) == SIG_ERR)
    {
        printf("can not catch SIGUSR2\n");
        exit(0);
    }
    
    sigset_t newmask, oldmask, pendmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        printf("add mask signal SIGUSR1 error.\n");
        exit(0);
    }
    printf("SIGUSR1 block\n");
    printf("begin sleep\n");
    sleep(30);
    printf("end sleep\n");
    
    if (sigpending(&pendmask) < 0)
    {
        printf("pending error.\n");
        exit(0);
    }
    
    if (sigismember(&pendmask, SIGUSR1))
    {
        printf("SIGUSR1 pending\n");
    }
    
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("remove mask SIGUSR1 error.\n");
        exit(0);
    }
    
    printf("unblock SIGUSR1\n");
    
    while (1)
    {
        pause();
    }
    exit(0);
}