#include "../inc/acceptor.h"
#include "../inc/server.h"
#include "../inc/socket.h"
#include "../inc/channel.h"
#include "../inc/inet_address.h"
#include <cstdio>

Acceptor::Acceptor(EventLoop *loop) :
    loop_(loop) {
    sock_ = new Socket();
    addr_ = new InetAddress("127.0.0.1", 8886);
    sock_->Bind(addr_);
    sock_->Listen();
    sock_->SetNonBlocking();
    accept_channel_ = new Channel(loop_, sock_->get_fd());
    printf("debug,Acceptior");
    auto callback = [this] { AcceptConnection(); };
    accept_channel_->set_callback_(callback);
    accept_channel_->EnableReading();
}

Acceptor::~Acceptor() {
    delete sock_;
    delete addr_;
    delete accept_channel_;
}

void Acceptor::AcceptConnection() {
    new_connection_callback_(sock_);
}

void Acceptor::set_new_connection_callback_(std::function<void(Socket *)> callback) {
    new_connection_callback_ = callback;
}