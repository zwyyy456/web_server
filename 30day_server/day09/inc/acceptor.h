#pragma once

#include "channel.h"
#include "socket.h"
#include <functional>
#include <type_traits>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
  private:
    EventLoop *loop_;
    Socket *sock_;
    Channel *accept_channel_;
    std::function<void(Socket *)> new_connection_callback_;

  public:
    Acceptor(EventLoop *loop);
    ~Acceptor();
    void AcceptConnection();
    void set_new_connection_callback_(std::function<void(Socket *)>);
};