//多线程信号测试 子线程会继承现有主线程的信号屏蔽字 每个线程都有自己的线程屏蔽字 但是会共享
//信号处理程序
#include "unp.h"

void *thread_func1(void *arg)
{
    sleep(2);
    sigset_t oldmask;
    sigemptyset(&oldmask);
    if (pthread_sigmask(0, NULL, &oldmask) < 0)
    {
        printf("thread1 0x%x sigmask error.\n", pthread_self());
        pthread_exit((void*)0);
    }
    if (sigismember(&oldmask, SIGUSR1))
    {
        printf("thread1 0x%x SIGUSR1 is block.\n", pthread_self());
    }
    else
    {
        printf("thread1 0x%x SIGUSR1 is unblock.\n", pthread_self());
    }
    pthread_exit((void*)0);
}

int main(int argc, char **argv)
{
    pthread_t       ptid1;
    int             err;
    sigset_t        newmask, oldmask;
    void*           status;
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    
    if (pthread_sigmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    //if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        printf("main thread 0x%x sigmask error.\n", pthread_self());
        exit(0);
    }
    
    if ((err = pthread_create(&ptid1, NULL, thread_func1, NULL)) != 0)
    {
        printf("main thread 0x%x create thread error:%s\n", pthread_self(), strerror(err));
        exit(0);
    }
    
    printf("main thread create thread 1 0x%x complete.\n", ptid1);
    
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