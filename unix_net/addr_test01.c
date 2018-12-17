//测试地址查询接口
#include "unp.h"

void func1(const char *addr);
void func2(const char *addr);
void func3();
void func4(const char *host, const char *server);
void func5(const char *servname, const char *protoname);
void func6(const short port, const char *protoname);
void printhostent(struct hostent *host);
void printaddrinfo(struct addrinfo *info);
void printservent(struct servent *serv);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage :%s <type> <arg>\n", argv[0]);
        exit(0);
    }
    int nType = atoi(argv[1]);
    switch (nType)
    {
    case 1:
        func1(argv[2]);
        break;
    case 2:
        func2(argv[2]);
        break;
    case 3:
        func3();
    case 4:
        if (argc != 4)
            break;
        func4(argv[2], argv[3]);
        break;
    case 5:
        if (argc == 3)
            func5(argv[2], NULL);
        else if (argc == 4)
            func5(argv[2], argv[3]);
        break;
    case 6:
        if (argc == 3)
            func6(atoi(argv[2]), NULL);
        else if (argc == 4)
            func6(atoi(argv[2]), argv[3]);
        break;
    default:
        break;
    }
    exit(0);
}

void func1(const char *addr)
{
    printf("func1:gethostbyname\n");
    struct hostent *host;
    host = gethostbyname(addr);
    if (host != NULL)
    {
        printhostent(host);
    }
    else
    {
        printf("host is null:%s\n", hstrerror(h_errno));
    }
    return;
}

void func2(const char *arg)
{
    printf("func1:gethostbyaddr\n");
    struct hostent *host;
    struct in_addr addr;
    int nRet;
    if ((nRet= inet_pton(AF_INET, arg, &addr)) < 0)
    {
        printf("inet_pton error:%s\n", strerror(errno));
        return;
    }
    else if (nRet == 0)
    {
        printf("inet_pton:valid addr\n");
        return;
    }
    host = gethostbyaddr(&addr, sizeof(struct in_addr), AF_INET);
    if (host != NULL)
        printhostent(host);
    else
        printf("host is null:%s\n", hstrerror(h_errno));
    return;
}

void func3()
{
    struct hostent *host;
    sethostent(0);
    while ((host = gethostent()) != NULL)
    {
        printhostent(host);
    }
    endhostent();
    return;
}

void func4(const char *host, const char *server)
{
    struct addrinfo *ailist, *aip, hint;
    int err;
    
    hint.ai_flags   = AI_PASSIVE;
    hint.ai_family  = AF_INET;
    hint.ai_socktype    = SOCK_STREAM;
    hint.ai_protocol    = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr    = NULL;
    hint.ai_next    = NULL;
    
    if ((err = getaddrinfo(host, server, &hint, &ailist)) != 0)
    {
        printf("getaddrinfo error:%s\n", gai_strerror(err));
        return;
    }
    
    for (aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        printf("******************\n");
        printaddrinfo(aip);
    }
    return;
}

void func5(const char *servname, const char *protoname)
{
    printf("getservbyname.\n");
    struct servent *serv;
    if ((serv = getservbyname(servname, protoname)) != NULL)
    {
        printservent(serv);
    }
    else
    {
        printf("getservbyname error:%s\n", hstrerror(h_errno));
    }
}
void func6(const short port, const char *protoname)
{
    printf("getservbyport.\n");
    struct servent *serv;
    if ((serv = getservbyport(htons(port), protoname)) != NULL)
    {
        printservent(serv);
    }
    else
    {
        printf("getservbyport error:%s\n", hstrerror(h_errno));
    }
}

void printhostent(struct hostent *host)
{
    char **ptr = NULL;
    printf("h_name:%s\n", host->h_name);
    ptr = host->h_aliases;
    while (*ptr != NULL)
    {
        printf("h_aliases:%s\n", *ptr++);
    }
    switch (host->h_addrtype)
    {
    case AF_INET:
        printf("h_addrtype:AF_INET\n");
        break;
    case AF_INET6:
        printf("h_addrtype:AF_INET6\n");
        break;
    default:
        printf("h_addrtype:unknown\n");
        break;
    }
    printf("h_length:%d\n", host->h_length);
    ptr = host->h_addr_list;
    while (*ptr != NULL)
    {
        printf("h_addr_list:%s\n", inet_ntoa(*(struct in_addr*)(ptr++)));
    }
}

void printaddrinfo(struct addrinfo *info)
{
    printf("family:");
    switch (info->ai_family)
    {
    case AF_INET:
        printf("inet\n");
        break;
    case AF_INET6:
        printf("inet6\n");
        break;
    case AF_UNIX:
        printf("unix\n");
        break;
    case AF_UNSPEC:
        printf("unspecified\n");
        break;
    default:
        printf("unknown\n");
    }
    printf("type:");
    switch (info->ai_socktype)
    {
    case SOCK_STREAM:
        printf("stream\n");
        break;
    case SOCK_DGRAM:
        printf("datagram\n");
        break;
    case SOCK_RAW:
        printf("raw\n");
        break;
    default:
        printf("unknown (%d)\n", info->ai_socktype);
    }
    printf("protocol:");
    switch (info->ai_protocol)
    {
    case 0:
        printf("default\n");
        break;
    case IPPROTO_TCP:
        printf("TCP\n");
        break;
    case IPPROTO_UDP:
        printf("UDP\n");
        break;
    case IPPROTO_RAW:
        printf("raw\n");
        break;
    default:
        printf("unknown (%d)\n", info->ai_protocol);
    }
    printf("flags:");
    int flags = info->ai_flags;
    if (flags == 0)
        printf("0\n");
    else if (flags & AI_PASSIVE)
        printf("passive\n");
    else if (flags & AI_CANONNAME)
        printf("canon\n");
    else if (flags & AI_NUMERICHOST)
        printf("numhost");
    
    printf("addrlen:%d\n", info->ai_addrlen);
    printf("canonname:%s\n", info->ai_canonname);
    
    if (info->ai_family == AF_INET)
    {
        sockaddr_in *addr = (sockaddr_in*)info->ai_addr;
        char buf[MAXLINE];
        inet_ntop(AF_INET, &addr->sin_addr, buf, MAXLINE);
        printf("address:%s,port:%d\n", buf, ntohs(addr->sin_port));
        //printf("address:%s %d\n",inet_ntop(info->ai_family, &((sockaddr_in*)info->ai_addr)->sin_addr, buf, sizeof(buf)), ntohs(((sockaddr_in*)info->ai_addr)->sin_port));
    }
}

void printservent(struct servent *serv)
{
    char **ptr;
    printf("server name:%s\n", serv->s_name);
    printf("server aliases:\n");
    for (ptr = serv->s_aliases; *ptr != NULL; ++ptr)
    {
        printf("%s\n", *ptr);
    }
    printf("port:%d\n", ntohs(serv->s_port));
}