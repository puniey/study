#include "unp.h"

typedef struct foo
{
    int a;
    int b;
    int c;
    int d;
}foo;

int gNum = 0;

void printfoo(const char *param, const foo *f)
{
    printf("%s:0x%x\n", param, f);
    printf("f.a=%d\n", f->a);
    printf("f.b=%d\n", f->b);
    printf("f.c=%d\n", f->c);
    printf("f.d=%d\n", f->d);
}

void printValue(const char *param)
{
    printf("%s:0x%x\n", param, &gNum);
    printf("value=%d\n", gNum);
}

void *thr_func1(void *arg)
{
    //static foo f = {1, 2, 3, 4};
    printf("this is thread 1:0x%x\n", pthread_self());
    //printfoo("thread 1", &f);
    //sleep(2);
    //printfoo("thread 1", &f);
    printValue("thread1");
    gNum++;
    printValue("thread1");
    //pthread_exit((void*)&f);
    pthread_exit((void*)0);
}

void *thr_func2(void *arg)
{
    printf("this is thread 2:0x%x\n", pthread_self());
    printValue("thread2");
    gNum++;
    printValue("thread2");
    pthread_exit((void*)0);
}

int main(int argc, char **argv)
{
    pthread_t ptid1, ptid2;
    int err = 0;
    //foo *f;
    
    printValue("main thread");
    printf("main thread create thread1\n");
    if ((err = pthread_create(&ptid1, NULL, thr_func1, NULL)) != 0)
    {
        printf("create thread1 error:%s\n", strerror(err));
        exit(0);
    }
    
    sleep(1);
    printf("main thread create thread2\n");
    if ((err = pthread_create(&ptid2, NULL, thr_func2, NULL)) != 0)
    {
        printf("create thread2 error:%s\n", strerror(err));
        exit(0);
    }
    
    //if ((err = pthread_join(ptid1, (void*)&f)) != 0)
    if ((err = pthread_join(ptid1, NULL)) != 0)
    {
        printf("join thread1 error:%s\n", strerror(err));
        exit(0);
    }
    
    //printfoo("main thread", f);
    printValue("main thread");
    sleep(1);
    exit(0);
}