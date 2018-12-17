#include "unp.h"

int tcp_listen(const char *host, const char *serv, socklen_t *addrlen)
{
    int     listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;
    char buf[125];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags  = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    
    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        printf("tcp_listen error for %s %s:%s\n", host, serv, gai_strerror(n));
        return -1;
    }
    
    ressave = res;
    
    while (res != NULL)
    {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;
        
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
            continue;
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        close(listenfd);
        res = res->ai_next;
    }
    if (res == NULL)
    {
        freeaddrinfo(ressave);
        printf("tcp_listen error for %s %s\n", host, serv);
        return -1;
    }
    else
    {
        listen(listenfd, LISTENQ);
        if (addrlen)
            *addrlen = res->ai_addrlen;
        /*printf("tcp_listen success for %s %s[%s:%d]\n", host, serv,
            inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, buf, sizeof(buf)), ntohs(((sockaddr_in*)res->ai_addr)->sin_port));*/
        freeaddrinfo(ressave);
        return listenfd;
    }
}
