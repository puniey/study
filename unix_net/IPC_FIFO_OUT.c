//进程间通信-命名管道
//从命名管道读取数据写入标准输出
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
    
    if ((fp = fopen(fifo, "r")) == NULL)
    {
        printf("open fifo file error:%s\n", strerror(errno));
        exit(0);
    }
    
    while (fgets(buf, MAXLINE, fp) != NULL)
    {
        if (fputs(buf, stdout) == EOF)
        {
            printf("fputs error.\n");
            exit(0);
        }
        fflush(stdout);
    }
    
    if (ferror(fp))
    {
        printf("get error from fp");
    }
    
    fclose(fp);
    exit(0);
}