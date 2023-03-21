#include "../inc/event_loop.h"
// #include "../inc/epoll.h"
#include "vector"

EventLoop::EventLoop() : ep_(nullptr), quit(false) {
    ep_ = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep_;
}

void EventLoop::Loop() {
    while (!quit) {
        std::vector<Channel *> chs;
        chs = ep_->Poll();
        for (auto it = chs.begin(); it != chs.end(); ++it) {
            (*it)->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *ch) {
    ep_->UpdateChannel(ch);
g
