#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include "locker.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class HttpConn {
  public:
    static int epoll_fd_; // 所有socket上的事件都被注册到同一个epoll对象中
    static int user_cnt_; // 统计用户数量
    HttpConn();
    ~HttpConn();

    void process(); // 处理客户端请求
  private:
    int sock_fd_;         // 该http连接的socket
    sockaddr_in address_; // 通信的socket地址
};

// 添加文件描述符到epoll中，
void add_fd(int epoll_fd, int fd, bool one_shot);
// 从epoll中删除文件描述符
void rm_fd(int epoll_fd, int fd);

void modfd(int epoll_fd, int fd, int ev);

#endif