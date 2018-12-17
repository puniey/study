#include "unp.h"

int udp_client(const char *host, const char *serv, sockaddr **addrptr, socklen_t *lenptr)
{
    int     sockfd, n;
    struct addrinfo hints, *res, *ressave;
    char    buf[125];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;
    
    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        printf("udp_client getaddrinfo for %s %s error:%s\n", host, serv, gai_strerror(n));
        return -1;
    }
    
    ressave = res;
    
    while (res != NULL)
    {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd >= 0)
            break;
        res = res->ai_next;
    }
    
    if (res == NULL)
    {
        printf("udp_client error for %s %s\n", host, serv);
        return -1;
    }
    
    printf("msg send to [%s:%d]\n", 
        inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, buf, sizeof(buf)),
        ntohs(((sockaddr_in*)res->ai_addr)->sin_port));
    
    *addrptr = (sockaddr*)malloc(res->ai_addrlen);
    memcpy(*addrptr, res->ai_addr, res->ai_addrlen);
    *lenptr = res->ai_addrlen;
    
    freeaddrinfo(ressave);
    return sockfd;
}