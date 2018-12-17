#include "unp.h"

struct addrinfo *host_serv(const char *hostname, const char *service, int family, int socktype)
{
    int     n;
    struct addrinfo hints, *res;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags  = AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype   = socktype;
    
    if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0)
        return NULL;
    return res;
}