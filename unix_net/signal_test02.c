//²âÊÔsigsuspendº¯Êý¹¦ÄÜ
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
    pr_mask("in sig_usr");
    return;
}

int main(int argc, char **argv)
{
    sigset_t    newmask, waitmask, oldmask;
    
    if (Signal(SIGUSR1, sig_usr) < 0)
    {
        printf("signal SIGUSR1 error.\n");
        exit(0);
    }
    
    if (Signal(SIGUSR2, sig_usr) < 0)
    {
        printf("signal SIGUSR1 error.\n");
        exit(0);
    }
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    //sigaddset(&waitmask, SIGUSR2);
    
    //if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    //{
        //printf("sig block error.\n");
        //exit(0);
    //}
    
    pr_mask("main");
    if (sigsuspend(&waitmask) != -1)
    {
        printf("sigsuspend error.\n");
        exit(0);
    }
    pr_mask("main");
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("sig block error.\n");
        exit(0);
    }
    exit(0);
}