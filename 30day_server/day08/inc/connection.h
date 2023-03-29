#pragma once

#include "channel.h"
#include "server.h"
#include "socket.h"
#include <functional>

class EventLoop;
class Socket;
class Channel;

class Connection {
  private:
    EventLoop *loop_;
    Socket *sock_;
    Channel *channel_;
    std::function<void(Socket *)> delete_connection_callback_;

  public:
    Connection(EventLoop *loop, Socket *sock);
    ~Connection();

    void Echo(int sockfd);
    void set_delete_connection_callback_(std::function<void(Socket *)>);
};