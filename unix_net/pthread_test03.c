//线程私有数据
#include "unp.h"

pthread_key_t   key;
pthread_once_t  init_once   = PTHREAD_ONCE_INIT;
pthread_mutex_t gMutex      = PTHREAD_MUTEX_INITIALIZER;

void destructor(void *arg)
{
    printf("thread 0x%x:call destructor\n", pthread_self());
    free(arg);
}

static void thread_init(void)
{
    int err;
    if ((err = pthread_key_create(&key, destructor)) != 0)
    {
        printf("thread 0x%x:thread init error:%s\n", pthread_self(), strerror(err));
    }
    else
    {
        printf("thread 0x%x:thread init success\n", pthread_self());
    }
}

void *thread_func(void *arg)
{
    int i = 0;
    while (1)
    {
        pthread_testcancel();
        int *pNum = NULL;
        
        pthread_once(&init_once, thread_init);
        pthread_mutex_lock(&gMutex);
        pNum = (int*)pthread_getspecific(key);
        if (pNum == NULL)
        {
            printf("thread 0x%x:pNum is null\n", pthread_self());
            pNum = (int*)malloc(sizeof(int));
            if (pNum == NULL)
            {
                printf("thread 0x%x:malloc error\n", pthread_self());
                pthread_mutex_unlock(&gMutex);
                pthread_exit((void*)0);
            }
            pthread_setspecific(key, pNum);
            printf("thread 0x%x:pNum malloc success 0x%x\n", pthread_self(), pNum);
        }
        else
        {
            printf("thread 0x%x:pNum is not null 0x%x\n", pthread_self(), pNum);
        }
        pthread_mutex_unlock(&gMutex);       
        sleep(1);        
    }
    pthread_exit((void*)0);
}

int main(int argc, char **argv)
{
    pthread_t       ptid1, ptid2;
    int             err;
    pthread_attr_t  attr;
    void            *status;
    
    printf("main thread create thread 1 2\n");
    if ((err = pthread_attr_init(&attr)) != 0)
    {
        printf("init thread attr error:%s\n", strerror(err));
        exit(0);
    }
    if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0)
    {
        printf("set detach attr error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        exit(0);
    }
    if ((err = pthread_create(&ptid1, &attr, thread_func, NULL)) != 0)
    {
        printf("main thread create thread 1 error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        exit(0);
    }
    if ((err = pthread_create(&ptid2, &attr, thread_func, NULL)) != 0)
    {
        printf("main thread create thread 2 error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        exit(0);
    }
    pthread_attr_destroy(&attr);
    printf("main thread create thread 1 0x%x complete.\n", ptid1);
    printf("main thread create thread 2 0x%x complete.\n", ptid2);
    sleep(5);
    if ((err = pthread_cancel(ptid1)) != 0)
    {
        printf("cancel thread 0x%x error:%s\n", ptid1, strerror(err));
    }
    else
    {
        printf("cancel thread 0x%x success\n", ptid1);
    }
    /*
    if ((err = pthread_join(ptid, &status)) != 0)
    {
        printf("main thread join thread 0x%x error:%s\n", ptid, strerror(err));
        exit(0);
    }
    */
    //printf("thread 1 0x%x exit code:%d\n", ptid, (int)status);
    //return 0;
    pthread_exit((void*)0);
}