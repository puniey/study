//¥Ê¥¢”≥…‰ mmap
#include "unp.h"

int main(int argc, char **argv)
{
    int     fd;
    void    *dst, *tmp;
    char    buf[MAXLINE];
    size_t  n;
    pid_t   pid;
    
    if ((fd = open("mmap.txt", O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)
    {
        printf("open file error\n");
        exit(0);
    }
    
    if (lseek(fd, MAXLINE - 1, SEEK_SET) == -1)
    {
        printf("lseek error\n");
        exit(0);
    }
    
    if (write(fd, "", 1) != 1)
    {
        printf("write error\n");
        exit(0);
    }
    
    if ((dst = mmap(0, MAXLINE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        printf("mmap error.\n");
        exit(0);
    }
    close(fd);
    tmp = dst;
    
    if ((pid = fork()) < 0)
    {
        printf("fork error:%s\n", strerror(errno));
        exit(0);
    }
    else if (pid == 0) //◊”Ω¯≥Ã
    {
        printf("this is child process\n");
        while (1)
        {
            printf("child %d:\n", getpid());
            memset(buf, 0, MAXLINE);
            if ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0)
            {
                memcpy(tmp, buf, n);
                tmp += n;
            }
            else
                break;
            sleep(1);
        }
    }
    else
    {
        printf("this is parent process\n");
        while (1)
        {
            printf("parent %d:\n", getpid());
            memset(buf, 0, MAXLINE);
            if ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0)
            {
                memcpy(tmp, buf, n);
                tmp += n;
            }
            else
                break;
            sleep(1);
        }
    }

    
    
    if (munmap(dst, MAXLINE) < 0)
    {
        printf("pid %d munmap error:%s\n", getpid(), strerror(errno));
    }
    else
    {
        printf("pid %d munmap success\n", getpid());
    }
    //close(fd);
    exit(0);
}