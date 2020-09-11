#ifndef EPOLL_H_
#define EPOLL_H_
#include <sys/epoll.h>
int EpollBuild(int *epoll_fd);
int EpollAddListener(int epoll_fd, struct epoll_event *epoll_event,int listen_fd,uint32_t events);
#endif
