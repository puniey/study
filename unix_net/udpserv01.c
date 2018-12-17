//UDP协议 回射服务器
#include "unp.h"
void dg_echo(int sockfd, sockaddr* cliaddr, size_t clilen);
void dg_echo2(int sockfd, sockaddr* cliaddr, size_t clilen);

static int count;
static void recvfrom_int(int signo)
{
    printf("\nreceive %d datagrams.\n", count);
    //exit(0);
}

int main(int argc, char **argv)
{
    int         sockfd;
    sockaddr_in servaddr, cliaddr;
    int         nVal = 0;
    int         nLen = sizeof(nVal);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error:");
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family         = AF_INET;
    servaddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    servaddr.sin_port           = htons(SERV_PORT);
    
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &nVal, &nLen);
    printf("udp recvbuf:%d\n", nVal);
    
    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind error:");
        exit(0);
    }
    
    dg_echo(sockfd, (sockaddr*)&cliaddr, sizeof(cliaddr));
    exit(0);
}

void dg_echo(int sockfd, sockaddr* cliaddr, size_t clilen)
{
    int         n;
    socklen_t   len;
    char        buf[MAXLINE];
    char        cliip[125];
    
    memset(cliip, 0, sizeof(cliip));
    
    while (1)
    {
        sleep(1);
        printf("begin receive\n");
        len = clilen;
        memset(buf, 0, sizeof(buf));
        n   = recvfrom(sockfd, buf, sizeof(buf), 0, cliaddr, &len);
        printf("recvfrom %d bytes from [%s:%d]:%s\n", n, inet_ntop(AF_INET, &((sockaddr_in*)cliaddr)->sin_addr, cliip, sizeof(cliip)), ntohs(((sockaddr_in*)cliaddr)->sin_port), buf);
        sendto(sockfd, buf, n, 0, cliaddr, len);
    }
}

void dg_echo2(int sockfd, sockaddr* cliaddr, size_t clilen)
{
    socklen_t   len;
    char        buf[MAXLINE];
    
    Signal(SIGINT, recvfrom_int);
    
    while (1)
    {
        len = clilen;
        recvfrom(sockfd, buf, MAXLINE, 0, cliaddr, &len);
        count++;
        sleep(1);
    }
}
