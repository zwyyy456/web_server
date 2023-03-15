#include "InetAddress.h"

#include <arpa/inet.h>
#include <cstring>

InetAddress::InetAddress() : addr_len_(sizeof(addr_)) {
    bzero(&addr_, addr_len_);
}

InetAddress::InetAddress(const char *ip_addr, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip_addr);
    addr_.sin_port = port;
    addr_len_ = sizeof(addr_);
}

InetAddress::~InetAddress() {
}