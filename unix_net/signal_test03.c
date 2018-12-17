//≤‚ ‘exit∫Õ_exit
#include "unp.h"

void func1(void)
{
    printf("pid %d call func1\n", getpid());
}

void func2(void)
{
    printf("pid %d call func2\n", getpid());
}

void func3(void)
{
    printf("pid %d call func3\n", getpid());
}

void sig_func(int signo)
{
    if (signo == SIGCHLD)
        printf("pid %d receive signal SIGCHLD.\n", getpid());
    else
        printf("pid %d receive signal %d\n", getpid(), signo);
}

int main(void)
{
    pid_t   pid;
    
    atexit(func1);
    atexit(func2);
    atexit(func3);
    
    Signal(SIGCHLD, sig_func);
    
    if ((pid = fork()) < 0)
    {
        printf("fork error.\n");
        exit(0);
    }
    else if (pid == 0)
    {
        printf("this is child process %d\n", getpid());
        //_exit(0);
        //return 0;
        //exit(0);
    }
    else
    {
        printf("this is parent process %d\n", getpid());
        //waitpid(pid, NULL, 0);
        //exit(0);
    }
    printf("main end\n");
    if (waitpid(pid, NULL, 0) < 0)
    {
        printf("pid %d waitpid error:%s.\n", getpid(), strerror(errno));
    }
    exit(0);
}