//exec测试
#include "unp.h"

int main(int argc, char **argv)
{
    printf("begin execl pid %d\n", getpid());
    pid_t   pid;
    int     stat;
    
    if ((pid = fork()) < 0)
    {
        printf("pid %d: fork error.\n", getpid());
        exit(0);
    }
    else if (pid == 0)
    {
        //if (execl("shell_test01", (char*)0) < 0)
        //if (execl("shell_test01", "111", "2222", (char*)0) < 0)
        //if (execl("/bin/ls", "ls", (char*)0) < 0) //执行shell指令
        //if (execl("/bin/bash", "bash", "shell_test02.sh", "arg1", "arg2", (char*)0) < 0)//执行shell脚本
        //if (execl("/bin/echo", "echo", "1", "2", (char*)0) < 0)
        //if (execl("echoargs", "echoargs", "1", "2", "3", (char*)0) < 0) //执行可执行文件
        //if (execl("/bin/bash", "bash", "-c", "ls", (char*)0) < 0) //执行shell指令
        //if (execl("/bin/bash", "bash", "-c", "/home/puniey/project/UnixNet/echoargs", (char*)0) < 0) //执行可执行文件
        /*if (execl("/bin/sh", "sh", "-c", "/home/puniey/project/UnixNet/test07", (char*)0) < 0) //执行可执行文件
        {
            printf("execl error:%s\n", strerror(errno));
            exit(0);
        }*/
        exit(11);
    }
    else
    {
        if (waitpid(pid, &stat, 0) < 0)
        {
            printf("pid %d: waitpid %d error.\n", getpid(), pid);
            exit(0);
        }
        printf("pid %d: child process %d is exit stat %d\n", getpid(), pid, WEXITSTATUS(stat));
    }
    
    exit(0);
}