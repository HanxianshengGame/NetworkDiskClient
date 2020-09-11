#include "../include/Epoll.h"
int EpollBuild(int *epoll_fd) {
    *epoll_fd = epoll_create(1);
    
    return *epoll_fd != -1 ? 0 : -1;
}
int EpollAddListener(int epoll_fd, struct epoll_event *epoll_event, int listen_fd,uint32_t events) {

    if((void*)0 == epoll_event) 
        return -1;
    epoll_event->data.fd = listen_fd;
    epoll_event->events = events;
    
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, epoll_event);
    
}
