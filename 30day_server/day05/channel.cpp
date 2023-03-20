#include "channel.h"
#include "epoll.h"

Channel::Channel(Epoll *ep, int fd) : ep_(ep), fd_(fd), events_(0), revents_(0), in_epoll_(false) {
}

Channel::~Channel() {
}

void Channel::EnableReading() {
    events_ = EPOLLIN | EPOLLET;
    ep_->UpdateChannel(this);
}

int Channel::get_fd() {
    return fd_;
}

uint32_t Channel::get_events() {
    return events_;
}

uint32_t Channel::get_revents() {
    return revents_;
}

bool Channel::get_in_epoll() {
    return in_epoll_;
}

void Channel::set_in_poll() {
    in_epoll_ = true;
}

void Channel::set_revents(uint32_t ev) {
    revents_ = ev;
}