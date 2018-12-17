//进程间通信-管道
//popen向另一个进程发送数据
#include "unp.h"

int main(void)
{
    char    line[MAXLINE];
    FILE    *fout;
    int     stat;
    
    //if ((fout = popen("/home/puniey/project/UnixNet/test07", "w")) == NULL)
    if ((fout = Popen("/home/puniey/project/UnixNet/test07", "w")) == NULL)
    {
        printf("pid %d popen error.\n", getpid());
        exit(0);
    }
    
    while (fgets(line, MAXLINE, stdin) != NULL)
    {
        if (fputs(line, fout) == EOF)
        //if (write(fileno(fout), line, strlen(line)) < 0)
        {
            printf("pid %d fputs error to pipe\n", getpid());
            exit(0);
        }
        fflush(fout);
    }
    
    if (ferror(stdin))
    {
        printf("pid %d fgets error from stdin\n", getpid());
        exit(0);
    }
    
    //if ((stat = pclose(fout)) == -1)
    if ((stat = Pclose(fout)) == -1)
    {
        printf("pid %d pclose pipe error\n", getpid());
        exit(0);
    }
    printf("stat:%d\n", WEXITSTATUS(stat));
    exit(0);
}