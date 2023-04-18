#include "socket.h"
#include "inet_address.h"
#include "util.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() : fd_(-1) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(fd_ == -1, "socket create error");
}

Socket::Socket(int fd) : fd_(fd) {
    ErrIf(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

void Socket::Bind(InetAddress *addr) {
    ErrIf(bind(fd_, (sockaddr *)&(addr->addr_), addr->addr_len_) == -1, "socket bind error");
}

void Socket::Listen() {
    ErrIf(listen(fd_, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::SetNonBlocking() {
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::Accept(InetAddress *clnt_addr) {
    int clnt_sockfd = accept(fd_, (sockaddr *)&(clnt_addr->addr_), &(clnt_addr->addr_len_));
    ErrIf(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

int Socket::get_fd() {
    return fd_;
}