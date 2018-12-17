#include "unp.h"

pthread_once_t init_done = PTHREAD_ONCE_INIT;

void test()
{
    printf("thread 0x%x:this is test()\n", pthread_self());
}

void *thread_func1(void *arg)
{
    printf("thread 1:0x%x\n", pthread_self());
    //test();
    pthread_once(&init_done, test);
    pthread_exit((void*)5);
}

void *thread_func2(void *arg)
{
    printf("thread 2\n");
    //test();
    pthread_once(&init_done, test);
    pthread_exit((void*)15);
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
    if ((err = pthread_create(&ptid1, &attr, thread_func1, NULL)) != 0)
    {
        printf("main thread create thread 1 error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        exit(0);
    }
    if ((err = pthread_create(&ptid2, &attr, thread_func1, NULL)) != 0)
    {
        printf("main thread create thread 2 error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        exit(0);
    }
    pthread_attr_destroy(&attr);
    printf("main thread create thread 1 0x%x complete.\n", ptid1);
    printf("main thread create thread 2 0x%x complete.\n", ptid2);
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