#include "epoll.h"
#include "util.h"

#include <cstring>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() {
    epfd_ = epoll_create1(0);
    ErrIf(epfd_ == -1, "epoll create error");
    events_ = new struct epoll_event[MAX_EVENTS];
    bzero(events_, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd_ != -1) {
        close(epfd_);
        epfd_ = -1;
    }
    delete[] events_;
}

void Epoll::AddFd(int fd, uint32_t op) {
    // fd即为sockfd
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    ErrIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

std::vector<epoll_event> Epoll::Poll(int timeout) {
    std::vector<epoll_event> active_events;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    ErrIf(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; i++) {
        active_events.push_back(events_[i]);
    }
    return active_events;
}
