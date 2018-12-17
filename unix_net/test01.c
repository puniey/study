#include "unp.h"
//modified not staged
void func1()
{
	printf("call func1\n");
}
void func2()
{
	printf("call func2\n");
}
void func3()
{
	printf("call func3\n");
}
int main(int argc, char** argv)
{
    char buf[1024];
    int n;
    
    memset(buf, 0, sizeof(buf));
   /* 
    while(1)
    {
        if ((n = readn(STDIN_FILENO, buf, MAXLINE)) > 0)
        {
            writen(STDOUT_FILENO, buf, n);
            //printf("read bytes:%d\n", n);
        }
        else if (n == 0)
        {
            //printf("read finished\n");
            continue;
        }
        else
        {
            //printf("read error, errno:%d\n", errno);
            return -1;
        }
    }
    printf("FD_SETSIZE:%d\n", FD_SETSIZE);
    printf("INADDR_ANY:%d\n", INADDR_ANY);
    printf("AF_INET:%d\n", AF_INET);
    printf("SOCK_STREAM:%d\n", SOCK_STREAM);
    printf("INFTIM:%d\n", INFTIM);
    printf("OPEN_MAX:%d\n", sysconf(_SC_OPEN_MAX));
    printf("BUFSIZE:%d\n", BUFSIZE);
	printf("sizeof cmsghdr:%d\n", sizeof(struct cmsghdr));
	*/
	atexit(func1);
	atexit(func2);
	atexit(func3);
	abort();
}
