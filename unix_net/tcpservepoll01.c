#include "unp.h"

typedef struct data{
    int sockfd;
    char buf[MAXLINE * 100];
    char *iptr;
    char *optr;
}data;

int main(int argc, char** argv)
{
    int i, n, m, listenfd, connfd, epfd, nRead, nWritten;
    sockaddr_in servaddr, cliaddr;
    epoll_event ev, events[MAXEVENTS];
    unsigned int event;
    socklen_t clilen;
    int value = 1;
    int value2 = 1;
    char cliip[20];
    int flag;
    int readError = 0;
    data *myData, *tmpData;
    char msg[MAXLINE * 100];
    char str[MAXLINE * 100];
    
    memset(msg, 0, sizeof(msg));
    memset(str, 0, sizeof(str));
    
    for (i = 0; i < 40000; ++i)
    {
        strcat(str, "hello");
    }
    sprintf(msg, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n%s", strlen(str), str);
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(-1);
    }
    
    if ((epfd = epoll_create(MAXEPOLL)) == -1)
    {
        perror("epoll_create");
        exit(-1);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(9880);
    
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    flag = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flag | O_NONBLOCK);
    
    if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind");
        exit(-1);
    }
    
    if (listen(listenfd, LISTENQ) == -1)
    {
        perror("listen");
        exit(-1);
    }
    
    myData         = (data*)malloc(sizeof(data));
    myData->sockfd = listenfd;
    myData->iptr   = myData->optr = myData->buf;
    ev.data.ptr    = myData;
    ev.events      = EPOLLIN;
    
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    {
        perror("epoll_ctl:listenfd");
        exit(-1);
    }
    
    while (1)
    {
        if ((n = epoll_wait(epfd, events, MAXEVENTS, -1)) == -1)
        {
            perror("epll_wait");
            exit(-1);
        }
        
        for (i = 0; i < n; ++i)
        {
            myData = (data*)events[i].data.ptr;
            event  = events[i].events;
            
            if (event & EPOLLIN)
            {
                if (myData->sockfd == listenfd)
                {
                    clilen = sizeof(cliaddr);
                    if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) == -1)
                    {
                        if (errno == EAGAIN)
                        {
                            continue;
                        }
                        else
                        {
                            perror("accept");
                            exit(-1);                            
                        }
                    }
                    flag = fcntl(connfd, F_GETFL, 0);
                    fcntl(connfd, F_SETFL, flag | O_NONBLOCK);
                    tmpData         = (data*)malloc(sizeof(data));
                    tmpData->sockfd = connfd;
                    tmpData->iptr   = tmpData->optr = tmpData->buf;
                    ev.data.ptr     = tmpData;
                    ev.events       = EPOLLIN | EPOLLET;
                    
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
                    {
                        perror("epoll_ctl:connfd");
                        exit(-1);
                    }
                }
                else
                {
                    readError = 0;
                    while (1)
                    {
                        if ((nRead =  read(myData->sockfd, myData->iptr, &myData->buf[MAXLINE] - myData->iptr)) <= 0)
                        {
                            if (errno == EAGAIN)
                            {
                                //perror("read EAGAIN");
                            }
                            else
                            {
                                perror("read");
                                if (epoll_ctl(epfd, EPOLL_CTL_DEL, myData->sockfd, NULL) == -1)
                                {
                                    perror("epoll_ctl:del sockfd");
                                    exit(-1);
                                }                                    
                                close(myData->sockfd);                       
                                readError = 1;                               
                            }
                            break;
                        }
                        else
                        {
                            myData->iptr += nRead;
                        }
                    }
                    
                    if (myData->optr < myData->iptr && readError == 0)
                    {
                        memset(myData->buf, 0, sizeof(myData->buf));
                        memcpy(myData->buf, msg, strlen(msg));
                        myData->optr = myData->buf;
                        myData->iptr = myData->buf + strlen(myData->buf);
                        ev.data.ptr = myData;
                        ev.events   = EPOLLOUT | EPOLLET;
                        if (epoll_ctl(epfd, EPOLL_CTL_MOD, myData->sockfd, &ev) == -1)
                        {
                            perror("epoll_ctl:mod sockfd");
                            exit(-1);
                        }
                    }
                    else if (readError == 1)
                    {
                        free(myData);
                    }
                }
            } 
            else if ((event & EPOLLOUT) && (m = (myData->iptr - myData->optr)) > 0)
            {
                while (1)
                {
                    if ((nWritten = write(myData->sockfd, myData->optr, m)) <= 0)
                    {
                        if (errno == EAGAIN)
                        {
                            perror("write EAGAIN");
                        }
                        else
                        {
                            perror("write");
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, myData->sockfd, NULL) == -1)
                            {
                                perror("epoll_ctl:del sockfd");
                                exit(-1);
                            }
                            close(myData->sockfd);
                            free(myData);
                        }
                        break;
                    }
                    else
                    {
                        myData->optr += nWritten;
                        if (myData->optr == myData->iptr)
                        {
                            /*
                            myData->optr = myData->iptr = myData->buf;
                            ev.data.ptr  = myData;
                            ev.events    = EPOLLIN | EPOLLET;
                            if (epoll_ctl(epfd, EPOLL_CTL_MOD, myData->sockfd, &ev) == -1)
                            {
                                perror("epoll_ctl:mod sockfd");
                                exit(-1);
                            }*/
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, myData->sockfd, NULL) == -1)
                            {
                                perror("epoll_ctl:DEL sockfd");
                                exit(-1);
                            }
                            close(myData->sockfd);
                            free(myData);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    close(epfd);
    
    return 0;
}