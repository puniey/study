//测试线程清理处理程序
#include "unp.h"

void *cleanup(void *arg)
{
    printf("cleanup:%s\n", (char*)arg);
}

void *thread_func1(void *arg)
{
    printf("thread 1 start\n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread 1 push complete\n");
    if (arg)
    {
        int *n = (int*)arg;
        (*n)++;
        printf("thread 1 arg=%d\n", (int)*n);
        //pthread_exit((void*)1);
        return (void*)1;
    }
    //else
    //{
        pthread_cleanup_pop(0);
        pthread_cleanup_pop(0);
        pthread_exit((void*)2);
    //}
}

void *thread_func2(void *arg)
{
    printf("thread 2 start\n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread 2 push complete\n");
    if (arg)
    {
        //pthread_cleanup_pop(0);
        int *n = (int*)arg;
        (*n)++;
        printf("thread 2 arg=%d\n", (int)*n);
        pthread_exit((void*)3);
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_exit((void*)4);
}

int main(int argc, char **argv)
{
    int err;
    pthread_t ptid1, ptid2;
    void *ret;
    int *n = (int*)malloc(sizeof(int));
    *n = 4;
    
    //if ((err = pthread_create(&ptid1, NULL, thread_func1, (void*)n)) != 0)
    if ((err = pthread_create(&ptid1, NULL, thread_func1, NULL)) != 0)
    {
        printf("create thread 1 error:%s\n", strerror(err));
        exit(0);
    }
    //sleep(1);
    //if ((err = pthread_create(&ptid2, NULL, thread_func2, (void*)n)) != 0)
    if ((err = pthread_create(&ptid2, NULL, thread_func2, NULL)) != 0)
    {
        printf("create thread 2 error:%s\n", strerror(err));
        exit(0);
    }
    
    if ((err = pthread_join(ptid1, &ret)) != 0)
    {
        printf("join thread 1 error:%s\n", strerror(err));
        exit(0);
    }
    
    printf("thread 1 exit code:%d\n", (int)ret);
    
    if ((err = pthread_join(ptid2, &ret)) != 0)
    {
        printf("join thread 2 error:%s\n", strerror(err));
        exit(0);
    }
    
    printf("thread 2 exit code:%d\n", (int)ret);
    exit(0);
}
