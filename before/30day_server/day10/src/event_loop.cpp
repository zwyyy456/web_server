#include "../inc/event_loop.h"
// #include "../inc/epoll.h"
#include "vector"

EventLoop::EventLoop() :
    ep_(nullptr), thread_pool_(nullptr), quit_(false) {
    ep_ = new Epoll();
    thread_pool_ = new ThreadPool();
}

EventLoop::~EventLoop() {
    delete ep_;
}

void EventLoop::Loop() {
    while (!quit_) {
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

void EventLoop::AddThread(std::function<void()> func) {
    thread_pool_->Add(func);
}
