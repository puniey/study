#include "unp.h"

void str_cli(int sockfd)
{
    char buf[MAXLINE];
	fd_set rset;
	FD_ZERO(&rset);
	int maxfdp1, stdineof, n, m;
    
    stdineof = 0;
    n        = 0;

	while(1)
	{
        memset(buf, 0, sizeof(buf));
        if (stdineof == 0)
		    FD_SET(STDIN_FILENO, &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = (STDIN_FILENO > sockfd ? STDIN_FILENO : sockfd) + 1;
        printf("maxfdp1 is %d.\n", maxfdp1);
		m = select(maxfdp1, &rset, NULL, NULL, NULL);
        printf("%d fd is ready to be read.\n", m);

		if (FD_ISSET(sockfd, &rset))
		{
			if ((n = read(sockfd, buf, sizeof(buf))) == 0)
			{
                if (stdineof == 1)
                {
                    printf("exit normal.\n");
                    return;
                }
                else
                {
				    printf("server terminated\n");
				    return;
                }
			}
            else if (n < 0)
            {
                printf("read sockfd error:%d\n", errno);
                return;
            }
            else
            {
                printf("tcpcli read %d bytes\n", n);
			    write(STDOUT_FILENO, buf, n);
            }
		}

		if (FD_ISSET(STDIN_FILENO, &rset))
		{
			if ((n = read(STDIN_FILENO, buf, sizeof(buf))) == 0)
            {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                //close(sockfd);
                FD_CLR(STDIN_FILENO, &rset);
                printf("finish read.\n");
                continue;
            }
            else if (n < 0)
            {
                printf("read stdin error:%d\n", errno);
                return;
            }
            else
            {
                printf("tcpcli write %d bytes\n", n);
			    write(sockfd, buf, n);
            }
		}

	}
}

int main(int argc, char **argv)
{
	int sockfd;
	sockaddr_in servaddr;
    struct linger ling;
    int nSendBuf, nRecvBuf, nLen;

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

    nLen = sizeof(nSendBuf);
    getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &nSendBuf, &nLen);
    nLen = sizeof(nRecvBuf);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &nRecvBuf, &nLen);
    printf("send buf size:%d\n", nSendBuf);
    printf("receive buf size:%d\n", nRecvBuf);
    
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    //printf("connect success.\n");
    //ling.l_onoff = 1;
    //ling.l_linger = 3;
    //setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    //close(sockfd);

	str_cli(sockfd);

	return 0;
}
