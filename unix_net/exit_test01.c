//≤‚ ‘exit∫Õ_exit
#include "unp.h"

int main(void)
{
    pid_t   pid;
    printf("begin fork.");
    if ((pid = vfork()) < 0)
    {
        printf("fork error.\n");
        exit(0);
    }
    else if (pid == 0)
    {
        //exit(0);
        return 0;
    }
    else
    {
        if (waitpid(pid, NULL, 0) < 0)
        {
            printf("waitpid error\n");
            exit(0);
        }
        exit(0);
    }
}