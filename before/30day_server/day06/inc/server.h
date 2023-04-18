#pragma once

#include "event_loop.h"
#include "socket.h"
class EventLoop;
class Socket;

class Server {
  private:
    EventLoop *loop_;

  public:
    Server(EventLoop *);
    ~Server();

    void HandleReadEvent(int);
    void NewConnenction(Socket *serv_sock);
};