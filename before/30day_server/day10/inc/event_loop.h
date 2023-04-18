#pragma once

#include "channel.h"
#include "epoll.h"
#include "thread_pool.h"

class Epoll;
class Channel;

class EventLoop {
  private:
    Epoll *ep_;
    ThreadPool *thread_pool_;
    bool quit_;

  public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void UpdateChannel(Channel *);

    void AddThread(std::function<void()> func);
};