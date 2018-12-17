#include "unp.h"

void str_cli(int);

int main(int argc, char **argv)
{
	int sockfd;
	sockaddr_in servaddr;

	if (argc != 3)
	{
		printf("usage: tcpcli <ip address> <port>");
		return -1;
	}
    
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

	str_cli(sockfd);

	return 0;
}

void str_cli(int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nRead, nWritten;
    fd_set rset, wset;
    char to[MAXLINE], fr[MAXLINE];
    char *toiptr, *tooptr, *friptr, *froptr;
    
    if ((val = fcntl(STDIN_FILENO, F_GETFL, 0)) < 0)
    {
        printf("fcntl STDIN_FILENO error.\n");
        return;
    }
    fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);
    
    if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
    {
        printf ("fcntl STDOUT_FILENO error.\n");
        return;
    }
    fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);
    
    if ((val = fcntl(sockfd, F_GETFL, 0)) < 0)
    {
        printf ("fcntl sockfd error.\n");
        return;
    }
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
    
    memset(to, 0, sizeof(to));
    memset(fr, 0, sizeof(fr));
    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;
    
    maxfdp1 = MAX(MAX(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;
    
    while(1)
    {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        
        if (stdineof == 0 && toiptr < &to[MAXLINE])
            FD_SET(STDIN_FILENO, &rset);
        if (friptr < &fr[MAXLINE])
            FD_SET(sockfd, &rset);
        if (tooptr < toiptr)
            FD_SET(sockfd, &wset);
        if (froptr < friptr)
            FD_SET(STDOUT_FILENO, &wset);
        
        select(maxfdp1, &rset, &wset, NULL, NULL);
        
        if (FD_ISSET(STDIN_FILENO, &rset))
        {
            if ((nRead = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    printf("read error on stdin.\n");
                    return;
                }
            }
            else if (nRead == 0)
            {
                stdineof = 1;
                printf("finish read.\n");
                if (toiptr == tooptr)
                {
                    printf("stdin send FIN.\n");
                    shutdown(sockfd, SHUT_WR);
                }
            }
            else
            {
                printf("read %d bytes from stdin\n", nRead);
                toiptr += nRead;
                FD_SET(sockfd, &wset);
            }
        }
        
        if (FD_ISSET(sockfd, &rset))
        {
            if ((nRead = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    printf("read error on sockfd.\n");
                    return;
                }
            }
            else if (nRead == 0)
            {
                if (stdineof)
                {
                    printf("get EOF on sockfd.\n");
                    return;
                }
                else
                {
                    printf("server terminated prematurely.\n");
                    return;                    
                }
            }
            else
            {
                printf("read %d bytes from sockfd.\n", nRead);
                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }
        
        if (FD_ISSET(sockfd, &wset) && (n = (toiptr - tooptr)) > 0)
        {  
            printf("begin write socket, n:%d\n", n);
            if ((nWritten = write(sockfd, tooptr, n)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    printf("write error on sockfd, errno:%d\n", n);
                    return;
                }
            }
            else
            {
                printf("write %d bytes to sockfd.\n", nWritten);
                tooptr += nWritten;
                if (tooptr == toiptr)
                {
                    tooptr = toiptr = to;
                    if (stdineof)
                    {
                        printf("finish write send FIN.\n");
                        shutdown(sockfd, SHUT_WR);
                    }
                }
            }
        }
        
        if (FD_ISSET(STDOUT_FILENO, &wset) && (n = (friptr - froptr)) > 0)
        {
            if ((nWritten = write(STDOUT_FILENO, froptr, n)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    printf("write error on stdout.errno:%d\n", errno);
                    return;
                }
            }
            else
            {
                printf("write %d bytes to stdout.\n", nWritten);
                froptr += nWritten;
                if (froptr == friptr)
                    froptr = friptr = fr;
            }
        }
    }
}