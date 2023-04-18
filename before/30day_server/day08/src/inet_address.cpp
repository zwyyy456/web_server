#include "../inc/inet_address.h"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

InetAddress::InetAddress() :
    addr_len_(sizeof(addr_)) {
    bzero(&addr_, addr_len_);
}

InetAddress::InetAddress(const char *ip_addr, uint16_t port) :
    addr_len_(sizeof(addr_)) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip_addr);
    addr_.sin_port = htons(port);
    addr_len_ = sizeof(addr_);
}

InetAddress::~InetAddress() = default;

void InetAddress::SetInetAddr(sockaddr_in addr, socklen_t addr_len) {
    addr_ = addr;
    addr_len_ = addr_len;
}

sockaddr_in InetAddress::get_addr_() {
    return addr_;
}

socklen_t InetAddress::get_addr_len_() {
    return addr_len_;
}