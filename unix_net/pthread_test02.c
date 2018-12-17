//使用递归互斥量 UNIX环境高级编程(二) 第十二章 同步属性有源代码
#include "unp.h"

typedef void *(*func)(void*);

int createthread(func*, void*);//创建一个分离式线程

int main(int argc, char **argv)
{}

int createthread(func* f, void* arg)
{
    int             err;
    pthread_t       ptid;
    pthread_attr_t  attr;
    
    if ((err = pthread_attr_init(&attr)) != 0)
    {
        printf("init thread attr error:%s\n", strerror(err));
        return err;
    }
    if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0)
    {
        printf("set thread detached error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        return err;
    }
    if ((err = pthread_create(&ptid, &attr, f, arg)) != 0)
    {
        printf("create thread error:%s\n", strerror(err));
        pthread_attr_destroy(&attr);
        return err;
    }
    pthread_attr_destroy(&attr);
    printf("create thread:0x%x\n", ptid);
    return 0;
}