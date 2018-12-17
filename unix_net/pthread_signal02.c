//多线程信号测试 每个线程都有自己的线程屏蔽字 但是会共享
//信号处理程序
#include "unp.h"

void sigfunc1(int signo)
{
    printf("thread 0x%x call sigfunc1.\n", pthread_self());
}

void sigfunc2(int signo)
{
    printf("thread 0x%x call sigfunc2.\n", pthread_self());
}

void *thread_func1(void *arg)
{
    //Signal(SIGUSR1, sigfunc2);
    while (1);
    pthread_exit((void*)0);
}

int main(int argc, char **argv)
{
    pthread_t       ptid1;
    int             err;
    void*           status;
    sigset_t        newmask, oldmask;
    
    Signal(SIGUSR1, sigfunc1);
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    
    
    if ((err = pthread_create(&ptid1, NULL, thread_func1, NULL)) != 0)
    {
        printf("main thread 0x%x create thread error:%s\n", pthread_self(), strerror(err));
        exit(0);
    }
    
    if (pthread_sigmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        printf("main thread 0x%x sigmask error.\n", pthread_self());
        exit(0);
    }
    
    printf("main thread 0x%x create thread 1 0x%x complete.\n", pthread_self(), ptid1);
    
    if ((err = pthread_join(ptid1, &status)) != 0)
    {
        printf("main thread join thread 0x%x error:%s\n", ptid1, strerror(err));
        exit(0);
    }
    printf("thread 0x%x exit code:%d\n", ptid1, (int)status);
    
    if (pthread_sigmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("main thread 0x%x sigmask error.\n", pthread_self());
        exit(0);
    }
    exit(0);
}