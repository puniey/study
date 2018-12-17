#include "unp.h"

ssize_t readn(int fd, void* buf, size_t count)
{
    size_t nRead, nLeft;
    char *p;
    
    p = buf;
    nLeft = count;
    int i = 0;
    
    while (nLeft > 0)
    {
        //printf("loop:%d\n", ++i);
        if ((nRead = read(fd, p, nLeft)) < 0)
        {
            //printf("readn error, errno:%d\n", errno);
            if (errno == EINTR)
                nRead = 0;
            else
                return -1;
        }
        else if (nRead == 0)
        {
            printf("readn finished.\n");
            break;
        }
        //printf("readn :%s", buf);
        nLeft -= nRead;
        p     += nRead;
    }
    
    return count - nLeft;
}

ssize_t writen(int fd, const void* buf, size_t count)
{
    size_t nWrite, nLeft;
    const char *p;
    
    p     = buf;
    nLeft = count;
    
    while (nLeft > 0)
    {
        if ((nWrite = write(fd, p, nLeft)) <= 0)
        {
            if (nWrite < 0 && errno == EINTR)
                nWrite = 0;
            else
                return -1;
        }
        
        nLeft -= nWrite;
        p     += nWrite;
    }
    
    return count;
}

ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t n;

#ifdef HAVE_MSGHDR_MSG_CONTROL
	union{
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	}control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	int newfd;
	msg.msg_accrights = (caddr_t)&newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len	= nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ((n = recvmsg(fd, &msg, 0)) <= 0)
	{
		*recvfd = -1;
		perror("recvmsg error");
		return n;
	}

#ifdef HAVE_MSGHDR_MSG_CONTROL
    if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
			cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
	{
		if (cmptr->cmsg_level != SOL_SOCKET)
		{
			printf("control level != SOL_SOCKET\n");
			exit(0);
		}
		if (cmptr->cmsg_type != SCM_RIGHTS)
		{
			printf("control type != SCM_RIGHTS\n");
			exit(0);
		}
		*recvfd = *(int*)CMSG_DATA(cmptr);
	}
	else
		*recvfd = -1;
#else
	if (msg.msg_accrightslen == sizeof(int))
		*recvfd = newfd;
	else
		*recvfd = -1;
#endif
    return n;
}

ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];

#ifdef HAVE_MSGHDR_MSG_CONTROL
    union{
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	}control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type  = SCM_RIGHTS;
	*((int*)CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t)&sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len  = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return sendmsg(fd, &msg, 0);
}

void err_quit(int err, const char *msg)
{
	printf("%s:%s\n", msg, strerror(err));
	exit(errno);
}
