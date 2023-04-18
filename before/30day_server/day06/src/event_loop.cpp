#include "../inc/event_loop.h"
// #include "../inc/epoll.h"
#include "vector"

EventLoop::EventLoop() :
    ep_(nullptr), quit(false) {
    ep_ = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep_;
}

void EventLoop::Loop() {
    while (!quit) {
        std::vector<Channel *> chs;
        chs = ep_->Poll();
        for (auto &ch : chs) {
            ch->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *ch) {
    ep_->UpdateChannel(ch);
}
