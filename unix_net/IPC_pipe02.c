//进程间通信-管道
//将文件复制到分页程序
#include "unp.h"

#define DEF_PAGER   "/bin/more"

int main(int argc, char **argv)
{
    int     n;
    int     fd[2];
    pid_t   pid;
    char    *pager, *arg;
    char    line[MAXLINE];
    FILE    *fp;
    
    if (argc != 2)
    {
        printf("usage:%s <filename>\n", argv[0]);
        exit(0);
    }
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("cannot open file %s\n", argv[1]);
        exit(0);
    }
    if (pipe(fd) < 0)
    {
        printf("pipe error\n");
        exit(0);
    }
    if ((pid = fork()) < 0)
    {
        printf("fork error.\n");
        exit(0);
    }
    else if (pid > 0)
    {
        close(fd[0]);
        
        while (fgets(line, MAXLINE, fp) != NULL)
        {
            n = strlen(line);
            if (write(fd[1], line, n) != n)
            {
                printf("pid %d write error to pipe.\n", getpid());
                exit(0);
            }
        }
            
        if (ferror(fp))
        {
            printf("pid %d fget error\n", getpid());
            exit(0);
        }
        
        close(fd[1]);
        printf("pid %d write finished\n", getpid());
        if (waitpid(pid, NULL, 0) < 0)
        {
            printf("pid %d wait pid error.\n", getpid());
            exit(0);
        }
        exit(0);
    }
    else
    {
        close(fd[1]);
        if (fd[0] != STDIN_FILENO)
        {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
            {
                printf("pid %d dup2 error\n", getpid());
                exit(0);
            }
            close(fd[0]);
        }
        
        if ((pager = getenv("PAGER")) == NULL)
        {
            pager = DEF_PAGER;
        }
        printf("pid %d pager:%s\n", getpid(), pager);
        
        if ((arg = strrchr(pager, '/')) != NULL)
        {
            arg++;
        }
        else
        {
            arg = pager;
        }
        
        if (execl(pager, arg, (char*)0) < 0)
        {
            printf("pid %d execl error for %s\n", getpid(), pager);
            exit(0);
        }
        exit(0);
    }
}