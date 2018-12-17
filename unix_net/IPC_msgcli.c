//进程间通信-消息队列
//客户端 打开消息队列 获取数据
#include "unp.h"

int main(int argc, char **argv)
{
    //printf("IPC_PRIVATE:%d\n", IPC_PRIVATE); //常量IPC_PRIVATE 0
    if (argc != 3)
    {
        printf("usage:%s <file> <arg>\n", argv[0]);
        exit(0);
    }
    key_t   key;
    key = ftok(argv[1], atoi(argv[2]));//相同的路径名和编号生成的key一样
    printf("create key:%d\n", key);
    
    int msgid;
    if ((msgid = msgget(key, 0)) == -1) //获取消息队列标识符
    {
        printf("msgget error:%s\n", strerror(errno));//消息队列不存在 出错返回
        exit(0);
    }
    printf("open msgid:%d\n", msgid);
    msgbuf msg;
    long type;
    
    while (1)
    {
        printf("input msg type:");
        scanf("%d", &type);
        
        if (msgrcv(msgid, &msg, MAXLINE, type, 0) == -1)
        {
            printf("msgrcv error:%s\n", strerror(errno));
            break;
        }
        printf("get msg,msg type:%d, buf:%s\n", msg.type, msg.buf);
    }
    
    exit(0);
}