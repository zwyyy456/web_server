#pragma once

#include "channel.h"
#include "epoll.h"

class Epoll;
class Channel;

class EventLoop {
  private:
    Epoll *ep_;
    bool quit;

  public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void UpdateChannel(Channel *);
};