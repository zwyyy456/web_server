#pragma once

#include "acceptor.h"
#include "event_loop.h"
#include "socket.h"
#include "connection.h"

#include <map>

class EventLoop;
class Socket;
class Connection;

class Server {
  private:
    EventLoop *loop_;
    Acceptor *acceptor_;
    std::map<int, Connection *> connections_;

  public:
    Server(EventLoop *);
    ~Server();

    // void HandleReadEvent(int);
    void NewConnenction(Socket *serv_sock);
    void DeleteConnection(Socket *sock);
};