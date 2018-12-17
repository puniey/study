//多线程信号同步 有一个线程负责处理信号 其余屏蔽信号
#include "unp.h"

int             gFlag;
sigset_t        gMask;
pthread_mutex_t gMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   gWait  = PTHREAD_COND_INITIALIZER;

void *thread_func1(void *arg)
{
    int err, signo;
    
    while (1)
    {
        if ((err = sigwait(&gMask, &signo)) != 0)
        {
            printf("sigwait error:%s\n", strerror(err));
            pthread_exit((void*)10);
        }
        
        switch (signo)
        {
        case SIGUSR1:
            printf("catch SIGUSR1\n");
            break;
        case SIGUSR2:
            pthread_mutex_lock(&gMutex);
            gFlag = 1;
            pthread_cond_signal(&gWait);
            pthread_mutex_unlock(&gMutex);
            pthread_exit((void*)5);
        default:
            printf("cathc signal %d\n", signo);
            pthread_exit((void*)0);
        }
    }
}

int main(int argc, char **argv)
{
    pthread_t       ptid1;
    int             err;
    void*           status;
    sigset_t        oldmask;
    
    sigemptyset(&gMask);
    sigaddset(&gMask, SIGUSR1);
    sigaddset(&gMask, SIGUSR2);
    
    if (pthread_sigmask(SIG_BLOCK, &gMask, &oldmask) < 0)
    {
        printf("main thread 0x%x sigmask error.\n", pthread_self());
        exit(0);
    }
    
    if ((err = pthread_create(&ptid1, NULL, thread_func1, NULL)) != 0)
    {
        printf("main thread 0x%x create thread error:%s\n", pthread_self(), strerror(err));
        exit(0);
    }
    
    printf("main thread 0x%x create thread 1 0x%x complete.\n", pthread_self(), ptid1);
    
    pthread_mutex_lock(&gMutex);
    while (gFlag == 0)
    {
        pthread_cond_wait(&gWait, &gMutex);
    }
    pthread_mutex_unlock(&gMutex);
    
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