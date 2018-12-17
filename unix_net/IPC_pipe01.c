//进程间通信-管道
#include "unp.h"

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
        close(pipefd[1]);
        printf("pid:%d, child pid:%d\n", getpid(), pid);
        while ((n = read(pipefd[0], buf, MAXLINE)) > 0)
        {
            write(STDOUT_FILENO, buf, n);
        }
        
        close(pipefd[0]);
        //exit(0);
    }
    else
    {
        close(pipefd[0]);
        printf("pid:%d, parent pid:%d\n", getpid(), getppid());
        while ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0)
        {
            write(pipefd[1], buf, n);
        }
        close(pipefd[1]);
        exit(0);
    }
    sleep(5);
    exit(0);
}