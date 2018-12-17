//进程间通信-消息队列
#include "unp.h"

int main(int argc, char **argv)
{
    printf("IPC_PRIVATE:%d\n", IPC_PRIVATE); //常量IPC_PRIVATE 0
    printf("MSGMAX:%d\n", MSGMAX); //一条消息的最大长度
    printf("MSGMNB:%d\n", MSGMNB); //一个队列的最大长度
    /*
    if (argc != 3)
    {
        printf("usage:%s <file> <arg>\n", argv[0]);
        exit(0);
    }
    key_t   key;
    key = ftok(argv[1], atoi(argv[2]));//相同的路径名和编号生成的key一样
    printf("create key:%d\n", key);
    
    int msgid;
    
    //if ((msgid = msgget(IPC_PRIVATE, 0)) == -1)//每次都生成一个新的消息队列
    //if ((msgid = msgget(key, IPC_CREAT)) == -1)//如果存在则返回msgid，不存在则生成一个新的消息队列
    //if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL)) == -1)//如果存在则出错EEXIST，不存在则生成一个新的消息队列
    //if ((msgid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL)) == -1)//IPC_PRIVATE总是生成一个新的消息队列
    if ((msgid = msgget(key, 0)) == -1)//如果存在则返回msgid,不存在则出错
    {
        printf("msgget error:%s\n", strerror(errno));
        exit(0);
    }
    printf("msgid:%d\n", msgid);
    //msgid = 1;
    ///*
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        printf("msgctl error:%s\n", strerror(errno));
        exit(0);
    }
    printf("remove msgid %d\n", msgid);
    //*/
    exit(0);
}