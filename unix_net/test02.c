#include "unp.h"

int main(int argc, char **argv)
{
    int i = 0;
    for (; i < argc; ++i)
    {
        printf("argc[%d]=%s\n", i, argv[i]);
    }
    
    printf("child max:%d\n", sysconf(_SC_CHILD_MAX));
    printf("OPEN max:%d\n", sysconf(_SC_OPEN_MAX));
    printf("thread stack size:%d\n", sysconf(_SC_THREAD_ATTR_STACKSIZE));
    printf("thread stack addr:%d\n", sysconf(_SC_THREAD_ATTR_STACKADDR));
    pthread_attr_t pAttr;
    int err;
    int nStackSize = 0;
    if ((err = pthread_attr_init(&pAttr)) != 0)
    {
        printf("init pAttr error:%s\n", strerror(err));
        exit(0);
    }
    if ((err = pthread_attr_getstacksize(&pAttr, &nStackSize)) != 0)
    {
        printf("get pAttr stacksize error:%s\n", strerror(err));
        exit(0);
    }
    printf("stacksize:%d\n", nStackSize);
    time_t nowTm = time(NULL);
    printf("now time :%u\n", nowTm);
    struct timeval now;
    gettimeofday(&now, NULL);
    printf("now time second :%u microsecond:%u\n", now.tv_sec, now.tv_usec);
    //printf("tick per second:%d\n", sysconf(_SC_CLK_TCK));
    exit(0);
}