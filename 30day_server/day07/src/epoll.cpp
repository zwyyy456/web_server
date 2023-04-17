#include "../inc/epoll.h"
#include "../inc/channel.h"
#include "../inc/util.h"

#include <cstring>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() :
    epfd_(-1), events_(nullptr) {
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

// std::vector<epoll_event> Epoll::Poll(int timeout) {
//     std::vector<epoll_event> active_events;
//     int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
//     ErrIf(nfds == -1, "epoll wait error");
//     for (int i = 0; i < nfds; i++) {
//         active_events.push_back(events_[i]);
//     }
//     return active_events;
// }

std::vector<Channel *> Epoll::Poll(int timeout) {
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    ErrIf(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel *)events_[i].data.ptr;
        ch->set_revents(events_[i].events);
        active_channels.push_back(ch);
    }
    return active_channels;
}

void Epoll::UpdateChannel(Channel *channel) {
    int fd = channel->get_fd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->get_events();
    if (!channel->get_in_epoll()) {
        ErrIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->set_in_poll();
    } else {
        ErrIf(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}