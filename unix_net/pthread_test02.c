//ʹ�õݹ黥���� UNIX�����߼����(��) ��ʮ���� ͬ��������Դ����
#include "unp.h"

typedef void *(*func)(void*);

int createthread(func*, void*);//����һ������ʽ�߳�

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