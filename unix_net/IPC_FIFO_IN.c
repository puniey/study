//进程间通信-命名管道
//从标准输入读取数据写入命名管道
#include "unp.h"

int main(void)
{
    FILE    *fp;
    char    *fifo = "fifo.txt";
    char    buf[MAXLINE];
    int     n;
    
    if (access(fifo, F_OK) == -1) //命名管道不存在新建一个
    {
        if (mkfifo(fifo, 0744) == -1)
        {
            printf("make fifo file error:%s\n", strerror(errno));
            exit(0);
        }
    }
    
    if ((fp = fopen(fifo, "w")) == NULL)
    {
        printf("open fifo file error:%s\n", strerror(errno));
        exit(0);
    }
    
    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (fputs(buf, fp) == EOF)
        {
            printf("fputs error.\n");
            exit(0);
        }
        fflush(fp);
    }
    
    if (ferror(stdin))
    {
        printf("get error from stdin");
    }
    
    fclose(fp);
    exit(0);
}