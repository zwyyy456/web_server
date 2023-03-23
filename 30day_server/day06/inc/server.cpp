#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <functional>
#include <unistd.h>
#include <vector>

#include "../inc/channel.h"
#include "../inc/epoll.h"
#include "../inc/event_loop.h"
#include "../inc/inet_address.h"
#include "../inc/socket.h"
#include "../inc/util.h"
#include "../inc/server.h"

#define READ_BUF 1024

Server::Server(EventLoop *loop) :
    loop_(loop) {
    // 初始化服务器
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->Bind(serv_addr);
    serv_sock->Listen();
    serv_sock->SetNonBlocking();

    // Channel,用来表示不同的事件类型
    Channel *serv_channel = new Channel(loop_, serv_sock->get_fd());
    // bind，c++库函数
    std::function<void()> callback = std::bind(&Server::NewConnenction, this, serv_sock);
    serv_channel->set_callback_(callback);
    serv_channel->EnableReading();
}

Server::~Server() {
}

void Server::HandleReadEvent(int sockfd) {
    char buf[READ_BUF];
    while (true) {
        // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            // 客户端正常中断，继续读取
            printf("continue reading\n");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            // 非阻塞IO，表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) {
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}

void Server::NewConnenction(Socket *serv_sock) {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->Accept(clnt_addr)); // 会发生内存泄漏
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->get_fd(), inet_ntoa(clnt_addr->addr_.sin_addr), ntohs(clnt_addr->addr_.sin_port));
    clnt_sock->SetNonBlocking();
    Channel *clnt_channel = new Channel(loop_, clnt_sock->get_fd());
    std::function<void()> callback = std::bind(&Server::HandleReadEvent, this, clnt_sock->get_fd());
    clnt_channel->set_callback_(callback);
    clnt_channel->EnableReading();
}