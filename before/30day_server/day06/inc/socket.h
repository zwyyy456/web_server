#pragma once
#pragma once
class InetAddress;
class Socket;

class Socket {
  public:
    Socket();
    Socket(int);
    ~Socket();

    void Bind(InetAddress *);
    void Listen();
    void SetNonBlocking();

    int Accept(InetAddress *);

    int get_fd();

  private:
    int fd_;
};