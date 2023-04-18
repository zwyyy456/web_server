#pragma once

#include "channel.h"
#include "server.h"
#include "socket.h"

#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
  private:
    EventLoop *loop_;
    Socket *sock_;
    Channel *channel_;
    std::function<void(Socket *)> delete_connection_callback_;

    std::string *in_buffer_;
    Buffer *read_buffer_;

  public:
    Connection(EventLoop *loop, Socket *sock);
    ~Connection();

    void Echo(int sockfd);
    void set_delete_connection_callback_(std::function<void(Socket *)>);
};