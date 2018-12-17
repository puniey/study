#include "unp.h"

CEpollTool::CEpollTool(){}

CEpollTool::~CEpollTool(){}

int CEpollTool::EpollCreate(int size)
{
    return epoll_create(size);
}

int CEpollTool::EpollAdd(int epfd, int sockfd, epoll_event &ev)
{
    return epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, ev);
}

int CEpollTool::EpollMode(int epfd, int sockfd, epoll_event &ev)
{
    return epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, ev);
}

int CEpollTool::EpollDel(int epfd, int sockfd, epoll_event &ev)
{
    return epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, ev);
}