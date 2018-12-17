#include "unp.h"
//使用两个互斥量
#define NHASH 29
#define HASH(fp) (((unsigned long)fp) % NHASH)
struct foo *fh[NHASH];

pthread_mutex_t hashLock = PTHREAD_MUTEX_INITIALIZER;
typedef struct foo{
    int             f_count;
    pthread_mutex_t f_lock;
    struct foo             *f_next;
    int             f_id;
}foo;

foo *foo_alloc()
{
    foo  *fp;
    int idx;
    
    if ((fp = (foo*)malloc(sizeof(foo))) != NULL)
    {
        fp->f_count = 1;
        if (pthread_mutex_init(&fp->f_lock, NULL) == 0)
        {
            free(fp);
            return NULL;
        }
        //idx = 
    }
    return fp;
}

void foo_hold(foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

void foo_release(foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count--;
    if (fp->f_count == 0)
    {
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        pthread_mutex_unlock(&fp->f_lock);
    }
}

int main(int argc, char **argv)
{
    foo *p;
    printf("0x%x\n", p);
    exit(0);
}