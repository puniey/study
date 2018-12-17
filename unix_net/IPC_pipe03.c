//利用管道实现进程间的同步
#include "unp.h"

static int pfd1[2], pfd2[2];

void TELL_WAIT(void)
{
    if (pipe(pfd1) || pipe(pfd2))
    {
        printf("pipe error\n");
        exit(0);
    }
}

void TELL_PARENT(pid_t pid)
{
    if (write(pfd2[1], "c", 1) != 1)
    {
        printf("TELL_PARENT write error\n");
        exit(0);
    }
}

void WAIT_PARENT(void)
{
    char c;
    
    if (read(pfd1[0], &c, 1) != 1)
    {
        printf("WAIT_PARENT read error\n");
        exit(0);
    }
    
    if (c != 'p')
    {
        printf("WAIT_PARENT error\n");
        exit(0);
    }
}

void TELL_CHILD(pid_t pid)
{
    if (write(pfd1[1], "p", 1) != 1)
    {
        printf("TELL_CHILD write error\n");
        exit(0);
    }
}

void WAIT_CHILD(void)
{
    char c;
    
    if (read(pfd2[0], &c, 1) != 1)
    {
        printf("WAIT_CHILD read error\n");
        exit(0);
    }
    
    if (c != 'c')
    {
        printf("WAIT_CHILD error\n");
        exit(0);
    }
}

void sigchld(int signo)
{
    pid_t   pid;
    
    while ((pid = waitpid(0, NULL, WNOHANG)) > 0)
    {
        printf("process %d exit.\n", pid);
    }
}

int main(void)
{
    pid_t   pid;
    
    if (Signal(SIGCHLD, sigchld) == SIG_ERR)
    {
        printf("cannot catch SIGCHLD.\n");
        exit(0);
    }
    
    TELL_WAIT();
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(0);
    }
    else if (pid > 0)
    {
        int n = 0;
        WAIT_CHILD();
        while (n < 10)
        {
            printf("pid %d:%d\n", getpid(), n++);
            sleep(1);
        }
        //TELL_CHILD(pid);
        exit(0);
    }
    else
    {
        int n = 0;
        //WAIT_PARENT();
        while (n < 10)
        {
            printf("pid %d:%d\n", getpid(), n++);
            sleep(1);
        }
        TELL_PARENT(getppid());
        exit(0);
    }
}