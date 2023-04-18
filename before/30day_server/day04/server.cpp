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
    ep->AddFd(serv_sock->get_fd(), EPOLLIN | EPOLLET); // 边沿触发
    printf("addFd\n");
    while (true) {
        std::vector<epoll_event> events = ep->Poll();
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == serv_sock->get_fd()) {
                // 有新的客户端连接
                InetAddress *clnt_addr = new InetAddress();                   // 会发生内存泄漏，没有delete
                Socket *clnt_sock = new Socket(serv_sock->Accept(clnt_addr)); // 会发生内存泄漏
                printf("accept client\n");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->get_fd(), inet_ntoa(clnt_addr->addr_.sin_addr), ntohs(clnt_addr->addr_.sin_port));
                clnt_sock->SetNonBlocking();
                ep->AddFd(clnt_sock->get_fd(), EPOLLIN | EPOLLET);
            } else if (events[i].events & EPOLLIN) {
                // 可读事件
                HandleReadEvent(events[i].data.fd);
            } else {
                printf("something else heapped\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void HandleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) {
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