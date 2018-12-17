//进程间通信-信号量 SYSTEM V标准
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
    int semid;
    int val;
    
    if ((semid = semget(key, 5, IPC_CREAT | IPC_EXCL | 0644)) == -1)
    {
        printf("semget error:%s\n", strerror(errno));
        exit(0);
    }
    
    printf("semid:%d\n", semid);
    
    if ((val = semctl(semid, 1, GETVAL)) == -1)
    {
        printf("get semval error:%s\n", strerror(errno));
    }
    else
    {
        printf("semval:%d\n", val);
    }
    
    if (semctl(semid, IPC_RMID, 0) == -1)
    {
        printf("semctl error:%s\n", strerror(errno));
        exit(0);
    }
    printf("remove semid %d\n", semid);
    exit(0);
}