#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd, readn;
    char recvbuf[MAXLINE];
    
    memset(recvbuf, 0, MAXLINE);
    
    while (1)
    {
        if ((readn = read(STDIN_FILENO, recvbuf, MAXLINE)) <= 0)
        {
            return -1;
        }
        else
        {
            printf("%d, %s\n", readn, recvbuf);
        }
    }
    
    return 0;
}