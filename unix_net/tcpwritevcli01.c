//writev 集中写 需自己分配内配 没有回收 内存泄漏
#include "unp.h"

void str_cli(int sockfd)
{
	struct iovec iov[IOV_MAX];
    int i, n, count;
	for (i = 0; i < IOV_MAX; i++)
	{
		iov[i].iov_base = calloc(MAXLINE, 1);
		iov[i].iov_len  = MAXLINE;
	}
    while (1)
    {
		i = 0;
		while (1)
		{
			n = read(STDIN_FILENO, iov[i].iov_base, iov[i].iov_len);
			i++;
			if (n > 0 )
			{
				continue;
		    }
	        else if (n < 0 && errno == EINTR)
			{
			    continue;
		    }
	        else if (n < 0)
			{
			    printf("error:str_cli read from stdin: %d\n", errno);
		        break;            
	        }
			else
		    {
				printf("read finish from stdin\n");
				count = writev(sockfd, iov, i);
				printf("writev %d bytes, %d record\n", count, i);
				break;
			}
		}
        /*
        n = read(sockfd, recvbuf, sizeof(recvbuf));
        if (n > 0 )
        {
            write(STDOUT_FILENO, recvbuf, n);
        }
        else if (n < 0 && errno == EINTR)
        {
            continue;
        }
        else if (n < 0)
        {
            printf("error:str_cli read from sockfd(%d): %d\n", sockfd, errno);
            break;            
        }
        else
        {
            printf("exit, read from sockfd(%d)\n", sockfd);
            perror("read from sockfd");
            break;
        }
		*/
    }
}

int main(int argc, char **argv)
{
    int         sockfd;
    int         n;
    sockaddr_in servaddr, cliaddr;
    char        addr[INET_ADDRSTRLEN];
    unsigned short       port;
    int         clilen;
    
    if (argc != 2)
    {
        printf("usage:tcpcli01 <IPaddress>\n");
        return -1;
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error:%d\n", errno);
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(addr, 0, sizeof(addr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    if ((n = connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        printf("connect error:%d\n", errno);
        return -1;
    }
    clilen = sizeof(cliaddr);
    getsockname(sockfd, (sockaddr*)&cliaddr, &clilen);
    inet_ntop(AF_INET, &cliaddr.sin_addr, addr, sizeof(addr));
    port = ntohs(cliaddr.sin_port);
    str_cli(sockfd);
    return 0;
}
