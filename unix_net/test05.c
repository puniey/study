//测试父子进程 文件描述符
#include "unp.h"

int main(void)
{
    pid_t   pid;
    char    buf[MAXLINE] = "write to stdout\n";
    
    write(STDOUT_FILENO, buf, sizeof(buf) - 1);
    printf("before fork\n");
    
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
    }
    else if (pid == 0)
    {
        int fd = open("./tmp.out", O_RDONLY);
        printf("this is child process,open file %d.\n", fd);
        sleep(5);
        close(fd);
        exit(0);
    }
    else
    {
        int fd = open("./test.c", O_RDONLY);
        printf("this is father process,open file %d.\n", fd);
        sleep(5);
        close(fd);
    }
    exit(0);
}