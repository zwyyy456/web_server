#pragma once
#include "epoll.h"

class Epoll;
class Channel {
  private:
    Epoll *ep_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    bool in_epoll_;

  public:
    Channel(Epoll *ep, int fd);
    ~Channel();

    void EnableReading();

    int get_fd();
    uint32_t get_events();
    uint32_t get_revents();
    bool get_in_epoll();
    void set_in_poll();
    void set_revents(uint32_t ev);
};