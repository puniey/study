#include "unp.h"
//使用互斥量保护数据结构
typedef struct foo{
    int             f_count;
    pthread_mutex_t f_lock;
}foo;

foo *foo_alloc()
{
    foo *fp;
    
    if ((fp = (foo*)malloc(sizeof(foo))) != NULL)
    {
        fp->f_count = 1;
        if (pthread_mutex_init(&fp->f_lock) == 0)
        {
            free(fp);
            return NULL;
        }
    }
    return fp;
}

void foo_hold(foo *fp)
{
    pthread_mutex_lock(&fp->lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->lock);
}

void foo_release(foo *fp)
{
    pthread_mutex_lock(&fp->lock);
    fp->f_count--;
    if (fp->f_count == 0)
    {
        pthread_mutex_unlock(&fp->lock);
        pthread_mutex_destory(&fp->lock);
        free(fp);
    }
    else
    {
        pthread_mutex_unlock(&fp->lock);
    }
}
