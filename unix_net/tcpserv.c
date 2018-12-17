#include "unp.h"

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[255];
	
	memset(buf, 0, sizeof(buf));

	while ((n = readn(sockfd, buf, 255)) > 0)
	    writen(sockfd, buf, n);

	if (n < 0)
		printf("read error\n");

	return;
}

int main(int argc, char **argv)
{
	int listenfd, connfd, flag;
	pid_t childpid;
	socklen_t clilen;
	sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9877);
    

	bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 5);
	//signal(SIGCHLD, sig_child);
    

	while (1)
	{
        printf("begin to accept\n");
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
        {
			if (errno == EAGAIN)
			{
				printf("EAGAIN\n");
				continue;
			}
			else
			{
				printf("accept error\n");
				return;
			}
		}
        printf("accept success\n");
        
		if ((childpid = fork()) == 0)
		{
		    printf("Iam child process, my pid:%d\n", getpid());
		    printf("Iam child process, my parent pid:%d\n", getppid());
			close(listenfd);
			str_echo(connfd);
            close(connfd)
			printf("child process exit, my pid:%d\n", getpid());
            return 0;
		}
		printf("Iam parent process, my pid :%d\n", getpid());
        
		close(connfd);
	}
	return 0;
}
