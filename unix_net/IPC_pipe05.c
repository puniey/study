#include "unp.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("pid %d:less arg.\n", getpid());
        exit(10);
    }
    
    int fd = atoi(argv[1]);
    printf("pid %d:get fd %d\n", getpid(), fd);
    char    buf[MAXLINE];
    int     n;
    
    while((n = read(fd, buf, MAXLINE)) > 0)
    {
        write(STDOUT_FILENO, buf, n);
    }
    
    if (n == 0)
    {
        printf("pid %d read end\n", getpid());
        close(fd);
        exit(10);
    }
    else
    {
        printf("pid %d read error:%s\n", getpid(), strerror(errno));
        exit(10);
    }
}