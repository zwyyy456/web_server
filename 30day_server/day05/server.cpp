#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "epoll.h"
#include "inet_address.h"
#include "socket.h"
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void SetNonBlocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void HandleReadEvent(int sockfd);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8886);
    serv_sock->Bind(serv_addr);
    serv_sock->Listen();
    Epoll *ep = new Epoll();
    serv_sock->SetNonBlocking();
    // ep->AddFd(serv_sock->get_fd(), EPOLLIN | EPOLLET); // 边沿触发
    // printf("addFd\n");
    Channel *serv_channel = new Channel(ep, serv_sock->get_fd());
    serv_channel->EnableReading();
    while (true) {
        printf("server start\n");
        std::vector<Channel *> active_channels = ep->Poll();
        int nfds = active_channels.size();
        for (int i = 0; i < nfds; ++i) {
            int chfd = active_channels[i]->get_fd();
            if (chfd == serv_sock->get_fd()) {
                // 新客户端连接
                InetAddress *clnt_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->Accept(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->get_fd(), inet_ntoa(clnt_addr->addr_.sin_addr), ntohs(clnt_addr->addr_.sin_port));
                clnt_sock->SetNonBlocking();
                Channel *clnt_channel = new Channel(ep, clnt_sock->get_fd());
                clnt_channel->EnableReading(); // 调用UpdateChannel();
            } else if (active_channels[i]->get_revents() & EPOLLIN) {
                // 可读事件
                HandleReadEvent(active_channels[i]->get_fd());
            } else {
                // 其他事件，之后的版本来实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void HandleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) { // 非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) {
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd); // 关闭该socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}