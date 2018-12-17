//进程间通信-管道
//测试子进程调用exec时能不能使用父进程的描述符

#include "unp.h"

void sigchld(int signo)
{
    pid_t   pid;
    int     nStatus;
    
    while ((pid = waitpid(0, &nStatus, WNOHANG)) > 0)
    {
        printf("process %d exit, status %d.\n", pid, WEXITSTATUS(nStatus));
    }
}

int main(void)
{
    int     pipefd[2];
    pid_t   pid;
    char    buf[MAXLINE];
    int     n;
    
    if (Signal(SIGCHLD, sigchld) == SIG_ERR)
    {
        printf("cannot catch SIGCHLD.\n");
        exit(0);
    }
    
    if (pipe(pipefd) < 0)
    {
        printf("create pipe error.\n");
        exit(0);
    }
    /*
    printf("read port:%d, size:%d\n", pipefd[0], fpathconf(pipefd[0], _PC_PIPE_BUF));
    printf("write port:%d, size:%d\n", pipefd[1], fpathconf(pipefd[1], _PC_PIPE_BUF));
    */
    
    if ((pid = fork()) < 0)
    {
        printf("fork error.\n");
        exit(0);
    }
    else if (pid > 0)
    {
        close(pipefd[0]);
        while ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0)
        {
            write(pipefd[1], buf, n);
        }
        
        if (n == 0)
        {
            printf("pid %d write end\n", getpid());
        }
        else
        {
            printf("pid %d write error :%s\n", strerror(errno));
        }
        close(pipefd[1]);
    }
    else
    {
        close(pipefd[1]);
        char sInfo[10];
        snprintf(sInfo, 10, "%d", pipefd[0]);
        execl("/home/puniey/project/UnixNet/IPC_pipe05", "IPC_pipe05", sInfo, (char*)0);
        _exit(11);
    }
    sleep(5);
    exit(0);
}