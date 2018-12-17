#include "unp.h"

void str_cli(int sockfd);
void *Sendto(void *arg);

static int fd;
static int stdendof;

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
    char recvbuf[MAXLINE];
    pthread_t tid;
    int readn;
    int error;
    
    fd       = sockfd;
    stdendof = 0;
    memset(recvbuf, 0, sizeof(MAXLINE));
    
    if ((error = pthread_create(&tid, NULL, Sendto, NULL)) > 0)
    {
        printf("pthread_create error:%d\n", error);
        exit(-1);
    }
    
    while(1)
    {
        if ((readn = read(fd, recvbuf, MAXLINE)) == -1 )
        {
            perror("str_cli read");
            exit(-1);
        }
        else if (readn == 0)
        {
            if (stdendof == 0)
            {
                perror("server terminated");
                break;
            }
            else
            {
                perror("normal exit");
                break;
            }
        }
        else
        {
            write(STDOUT_FILENO, recvbuf, readn);
        }
    }
}

void *Sendto(void *arg)
{
    char sendbuf[MAXLINE];
    int readn;
    
    memset(sendbuf, 0, sizeof(MAXLINE));
    
    while(1)
    {
        if ((readn = read(STDIN_FILENO, sendbuf, MAXLINE)) < 0)
        {
            perror("read");
            exit(-1);
        }
        else if (readn == 0)
        {
            stdendof = 1;
            shutdown(fd, SHUT_WR);
            break;
        }
        else
        {
            write(fd, sendbuf, readn);
        }
    }
    return NULL;
}