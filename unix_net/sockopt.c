//getsockopt setsockopt
#include "unp.h"

union uval
{
    int             i_val;
    long            l_val;
    struct linger   linger_val;
    struct timeval  timeval_val; 
}val;

static char *sock_str_flag(union uval*, int);
static char *sock_str_int(union uval*, int);
static char *sock_str_linger(union uval*, int);
static char *sock_str_timeval(union uval*, int);
static char gBuf[128];

struct ssock_opt
{
    const char  *opt_str;
    int         opt_level;
    int         opt_name;
    char        *(*opt_val_str)(union uval*, int);
}sock_opts[] = {
    {"SO_BROADCAST",    SOL_SOCKET, SO_BROADCAST,   sock_str_flag},
    {"SO_DEBUG",        SOL_SOCKET, SO_DEBUG,       sock_str_flag},
    {"SO_DONTROUTE",    SOL_SOCKET, SO_DONTROUTE,   sock_str_flag},
    {"SO_ERROR",        SOL_SOCKET, SO_ERROR,       sock_str_int},
    {"SO_KEEPALIVE",    SOL_SOCKET, SO_KEEPALIVE,   sock_str_flag},
    {"SO_RCVBUF",       SOL_SOCKET, SO_RCVBUF,      sock_str_int},
    {"SO_SNDBUF",       SOL_SOCKET, SO_SNDBUF,      sock_str_int},
    {"SO_RCVLOWAT",     SOL_SOCKET, SO_RCVLOWAT,    sock_str_int},
    {"SO_SNDLOWAT",     SOL_SOCKET, SO_SNDLOWAT,    sock_str_int},    
    {NULL,              0,          0,              NULL}
};

int main(int argc, char **argv)
{
    int                 fd;
    socklen_t           len;
    struct ssock_opt    *ptr;
    
    for (ptr = sock_opts; ptr->opt_str != NULL; ++ptr)
    {
        printf("%s:", ptr->opt_str);
        if (ptr->opt_val_str == NULL)
            printf("undefined\n");
        else
        {
            switch (ptr->opt_level)
            {
            case SOL_SOCKET:
            case IPPROTO_IP:
            case IPPROTO_TCP:
                fd = socket(AF_INET, SOCK_STREAM, 0);
                break;
#ifdef IPV6
            case IPPROTO_IPV6:
                fd = socket(AF_INET6, SOCK_STREAM, 0);
                break;
#endif
#ifdef IPPROTO_SCTP
            case IPPROTO_SCTP:
                fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
                break;
#endif
            default:
                printf("cannot create fd for level %d\n", ptr->opt_level);
                exit(0);
            }
            
            len = sizeof(val);
            if (getsockopt(fd, ptr->opt_level, ptr->opt_name, &val, &len) == -1)
            {
                printf("getsockopt error:%s\n", strerror(errno));
            }
            else
            {
                printf("default=%s\n", (*ptr->opt_val_str)(&val, len));    
            }
            close(fd);
        }
    }
    exit(0);
}

static char  *sock_str_flag(union uval *ptr, int len)
{
    if (len != sizeof(int))
        snprintf(gBuf, sizeof(gBuf), "size (%d) not sizeof(int)", len);
    else
        snprintf(gBuf, sizeof(gBuf), "%d(%s)", ptr->i_val, ptr->i_val == 1?"on":"off");
    return gBuf;
}

static char *sock_str_int(union uval *ptr, int len)
{
    if (len != sizeof(int))
        snprintf(gBuf, sizeof(gBuf), "size (%d) not sizeof(int)", len);
    else
        snprintf(gBuf, sizeof(gBuf), "%d", ptr->i_val);
    return gBuf;
}

static char *sock_str_linger(union uval *ptr, int len)
{
    return "";
}

static char *sock_str_timeval(union uval *ptr, int len)
{
    return "";
}

