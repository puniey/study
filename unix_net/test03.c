#include "unp.h"
int gNum = 1000;
pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
void *pthread_func1(void *arg)
{
    //printf("new thread:pid:%d, ptrhead id:0x%x, global ptid:0x%x\n", getpid(), pthread_self(), ptid);
    //return ((void*)5);
    if (pthread_mutex_trylock(&gMutex) == 0)
    {
        printf("thread 1 lock success\n");
        printf("thread 1 gNum:%d\n", gNum);
        gNum++;
        sleep(2);
        printf("thread 1 gNum:%d\n", gNum);
        pthread_mutex_unlock(&gMutex);
    }
    else
        printf("thread 1 lock failed\n");
    pthread_exit((void*)10);
}

void *pthread_func2(void *arg)
{
    //printf("new thread:pid:%d, ptrhead id:0x%x, global ptid:0x%x\n", getpid(), pthread_self(), ptid);
    //return ((void*)5);
    if (pthread_mutex_trylock(&gMutex) == 0)
    {
        printf("thread 2 lock success\n");
        printf("thread 2 gNum:%d\n", gNum);
        gNum++;
        sleep(1);
        printf("thread 2 gNum:%d\n", gNum);
        pthread_mutex_unlock(&gMutex);
    }
    else
        printf("thread 2 lock failed\n");
    
    pthread_exit((void*)5);
}

int main(int argc, char **argv)
{
    pthread_t ptid1, ptid2;
    int err  = 0;
    void* nStatus;
    
    if ((err = pthread_create(&ptid1, NULL, pthread_func1, NULL)) != 0)
    {
        printf("create thread error:%s\n", strerror(err));
        exit(0);
    }
    
    if ((err = pthread_create(&ptid2, NULL, pthread_func2, NULL)) != 0)
    {
        printf("create thread error:%s\n", strerror(err));
        exit(0);
    }
    
    //printf("main thread:pid:%d, ptrhead id:0x%x, create thread:0x%x\n", getpid(), pthread_self(), ptid);
    //sleep(2);
    
    if ((err = pthread_join(ptid1, &nStatus)) != 0)
    {
        printf("can not join with thread 0x%x:%s\n", ptid1, strerror(err));
        exit(0);
    }
    printf("thread 1 0x%x exit code:%d\n", ptid1, (int)nStatus);
    if ((err = pthread_join(ptid2, &nStatus)) != 0)
    {
        printf("can not join with thread 0x%x:%s\n", ptid2, strerror(err));
        exit(0);
    }
    printf("thread 2 0x%x exit code:%d\n", ptid2, (int)nStatus);
    printf("main thread gNum:%d\n", gNum);
    return 0;
}