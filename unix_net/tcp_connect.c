#include "unp.h"

int tcp_connect(const char *host, const char *serv)
{
    int         sockfd, n;
    struct addrinfo hints, *res, *ressave;
    char        buf[125];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    
    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        printf("tcp_connect error for %s %s:%s\n", host, serv, gai_strerror(n));
        return -1;
    }
    
    ressave = res;
    while (res != NULL)
    {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        close(sockfd);
        res = res->ai_next;
    }
    
    if (res == NULL)
    {
        freeaddrinfo(ressave);
        printf("tcp_connect error for %s %s\n", host, serv);
        return -1;
    }
    else
    {
        /*printf("tcp_connect success for %s %s[%s:%d]\n", host, serv, 
            inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, buf, sizeof(buf)), ntohs(((sockaddr_in*)res->ai_addr)->sin_port));*/
        freeaddrinfo(ressave);
        return sockfd;
    }
}
