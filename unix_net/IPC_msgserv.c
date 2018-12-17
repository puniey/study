//进程间通信-消息队列
//服务端 创建消息队列 放入消息
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
    if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0644)) == -1)
    {
        printf("msgget error:%s\n", strerror(errno));
        exit(0);
    }
    printf("open msgid:%d\n", msgid);
    char buf[MAXLINE] = {0};
    msgbuf msg;
    
    while (read(STDIN_FILENO, buf, MAXLINE) != -1)
    {
        if (sscanf(buf, "%d%s", &msg.type, msg.buf) == EOF)
        {
            printf("input error:%s\n", strerror(errno));
            break;
        }
        if (msgsnd(msgid, &msg, MAXLINE, 0) == -1)
        {
            printf("msgsnd error:%s\n", strerror(errno));
            break;
        }
        printf("server:msgsnd type:%d,buf:%s\n", msg.type, msg.buf);
    }
    
    printf("input finished.\n");
    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        printf("remove msgid %d error:%s\n", msgid, strerror(errno));
    }
    else
    {
        printf("remove msgid %d success\n", msgid);
    }
    exit(0);
}