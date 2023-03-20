#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll;

class Epoll {
  public:
    Epoll();
    ~Epoll();
    void AddFd(int fd, uint32_t op);
    std::vector<epoll_event> Poll(int timeout = -1);

  private:
    int epfd_;
    struct epoll_event *events_;
};
