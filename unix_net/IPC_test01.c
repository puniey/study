//父子进程间通信-信号实现
#include "unp.h"

int main(int argc, char **argv)
{
    pid_t pid;
    TELL_WAIT();
    
    if ((pid = fork()) < 0)
    {
        printf("fork error.\n");
        exit(0);
    }
    else if (pid == 0) //子进程
    {
        WAIT_PARENT();
        int i = 0;
        while (i < 10)
        {
            printf("child process:%d\n", i++);
            sleep(1);
        }
    }
    else
    {
        int i = 0;
        while (i < 10)
        {
            printf("parent process:%d\n", i++);
            sleep(1);
        }
        TELL_CHILD(pid);
    }
    exit(0);
}