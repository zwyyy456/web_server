#pragma once
#include "epoll.h"
#include "event_loop.h"
#include <cstdint>
#include <functional>

class EventLoop;

class Epoll;
class Channel {
  private:
    EventLoop *loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    bool in_epoll_;
    std::function<void()> callback_;

  public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    void HandleEvent();
    void EnableReading();

    int get_fd();
    uint32_t get_events();
    uint32_t get_revents();
    bool get_in_epoll();
    void set_in_poll();
    void set_revents(uint32_t ev);
    void set_callback_(std::function<void()>);
};
