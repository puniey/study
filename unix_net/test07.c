#include "unp.h"

int main(void)
{
    char    line[MAXLINE];
    int     n;
    
    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0)
    {
        write(STDOUT_FILENO, line, n);   
    }
    printf("pid %d ppid %d exit\n", getpid(), getppid());
    exit(11);
}