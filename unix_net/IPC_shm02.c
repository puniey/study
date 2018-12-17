//进程间通信-共享内存
//服务器进程
#include "unp.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("usage:%s <file> <arg>\n", argv[0]);
        exit(0);
    }
    key_t   key;
    key = ftok(argv[1], atoi(argv[2]));//相同的路径名和编号生成的key一样
    printf("create key:%d\n", key);
    
    int shmid;
    if ((shmid = shmget(key, 0, 0)) < 0)
    {
        printf("shmget error:%s\n", strerror(errno));
        exit(0);
    }
    printf("shmget success:%d\n", shmid);
    sleep(10);
    char *shmptr;
    if ((shmptr = (char*)shmat(shmid, 0, 0)) == (void*)-1)
    {
        printf("shmat %d error:%s\n", shmid, strerror(errno));
        exit(0);
    }
    
    printf("attach shmid %d 0x%x\n", shmid, shmptr);
    ///*
    if (shmdt(shmptr) < 0)
    {
        printf("detach 0x%x error:%s\n", shmptr, strerror(errno));
    }
    //*/
    ///*
    if (shmctl(shmid, IPC_RMID, 0) < 0)
    {
        printf("remove shmid %d error:%s\n", shmid, strerror(errno));
        exit(0);
    }
    printf("remove shmid %d success\n", shmid);
    //*/
    exit(0);
}