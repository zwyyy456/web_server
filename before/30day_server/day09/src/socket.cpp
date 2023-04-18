#include "../inc/socket.h"
#include "../inc/inet_address.h"
#include "../inc/util.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() :
    fd_(-1) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(fd_ == -1, "socket create error");
}

Socket::Socket(int fd) :
    fd_(fd) {
    ErrIf(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

void Socket::Bind(InetAddress *_addr) {
    struct sockaddr_in addr = _addr->get_addr_();
    socklen_t addr_len = _addr->get_addr_len_();
    ErrIf(bind(fd_, (sockaddr *)&addr, addr_len) == -1, "sock bind error");
    _addr->SetInetAddr(addr, addr_len); // 似乎没有必要
}

void Socket::Listen() {
    ErrIf(listen(fd_, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::SetNonBlocking() {
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::Accept(InetAddress *clnt_addr) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    bzero(&addr, sizeof(addr));
    int clnt_sockfd = accept(fd_, reinterpret_cast<sockaddr *>(&addr), &addr_len); // 这里会保存要连接的客户端addr和addr_len
    ErrIf(clnt_sockfd == -1, "socket accept error");
    clnt_addr->SetInetAddr(addr, addr_len);
    return clnt_sockfd;
}

void Socket::Connect(InetAddress *_addr) {
    struct sockaddr_in addr = _addr->get_addr_();
    socklen_t addr_len = _addr->get_addr_len_();
    ErrIf(connect(fd_, reinterpret_cast<sockaddr *>(&addr), addr_len) == -1, "socket connect error");
}

int Socket::get_fd() {
    return fd_;
}