#include "../inc/acceptor.h"
#include "../inc/server.h"
#include "../inc/socket.h"
#include "../inc/channel.h"
#include "../inc/inet_address.h"
#include <cstddef>
#include <cstdio>

Acceptor::Acceptor(EventLoop *loop) :
    loop_(loop), sock_(nullptr), accept_channel_(nullptr) {
    sock_ = new Socket();
    InetAddress *addr_ = new InetAddress("127.0.0.1", 8886);
    sock_->Bind(addr_);
    sock_->Listen();
    sock_->SetNonBlocking();
    accept_channel_ = new Channel(loop_, sock_->get_fd());
    // printf("debug,Acceptior");
    auto callback = [this] { AcceptConnection(); };
    accept_channel_->set_callback_(callback);
    accept_channel_->EnableReading();

    delete addr_;
}

Acceptor::~Acceptor() {
    delete sock_;
    // delete addr_;
    delete accept_channel_;
}

void Acceptor::AcceptConnection() {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock_->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->get_fd(), inet_ntoa(clnt_addr->get_addr_().sin_addr), ntohs(clnt_addr->get_addr_().sin_port));
    clnt_sock->SetNonBlocking();
    new_connection_callback_(clnt_sock);
    delete clnt_addr;
}

void Acceptor::set_new_connection_callback_(std::function<void(Socket *)> callback) {
    new_connection_callback_ = callback;
}