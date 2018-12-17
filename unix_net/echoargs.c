//Êä³ö²ÎÊı
#include "unp.h"

int main(int argc, char **argv)
{
    int i = 0;
    printf("echoarg pid %d ppid %d\n", getpid(), getppid());
    for (; i < argc; ++i)
    {
        printf("argv[%d]:%s\n", i, argv[i]);
    }
    exit(0);
}