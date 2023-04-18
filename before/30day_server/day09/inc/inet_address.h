#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

class InetAddress;

class InetAddress {
  private:
    struct sockaddr_in addr_;
    socklen_t addr_len_;

  public:
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();

    void SetInetAddr(sockaddr_in addr, socklen_t addr_len);
    sockaddr_in get_addr_();
    socklen_t get_addr_len_();
};