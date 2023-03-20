#include "util.h"

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void SetNonBlocking(int fd) { // 设置成非阻塞，F_SETFL表示设置
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4/ipv6，标识流格式、面向连接，协议0表示自动推导
    ErrIf(sockfd == -1, "socket create error\n");

    // 服务器IP地址设定
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8889);

    ErrIf(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    ErrIf(listen(sockfd, SOMAXCONN) == -1, "socket listen error\n");

    // epoll相关配置
    int epfd = epoll_create1(0);
    ErrIf(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS];
    struct epoll_event ev;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    // ev.events = EPOLLIN | EPOLLET; //边沿触发
    ev.events = EPOLLIN;
    SetNonBlocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    // 保存连接的客户端的socket地址信息, epoll IO 复用模式下不需要
    // struct sockaddr_in client_addr;
    // socklen_t client_addr_len = sizeof(client_addr);
    // bzero(&client_addr, client_addr_len);

    // int client_sockfd = accept(sockfd, (sockaddr *)&client_addr, &client_addr_len);
    // printf("new client fd %d :: IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // 读取数据
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // -1 表示会一直等下去
        // 基本思想为：在创建了服务器socket fd后，将这个fd添加到epoll，只要这个fd上发生可读事件，表示有一个新的客户端连接。然后accept这个客户端并将客户端的socket fd添加到epoll，epoll会监听客户端socket fd是否有事件发生，如果发生则处理事件。
        ErrIf(nfds == -1, "epoll wait error");
        for (int i = 0; i < nfds; ++i) {
            // 说明有新客户端连接上来了
            if (events[i].data.fd == sockfd) {
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len);
                ErrIf(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET; // 对客户端连接，采用ET模式，可以使epoll更加高效，支持更多并发
                SetNonBlocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
                // 可读事件
            } else if (events[i].events & EPOLLIN) {
                char buf[READ_BUFFER];
                // 由于使用了非阻塞IO，读取客户端buffer，回一次读取buf大小的数据，直到全部读取完毕
                while (true) {
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if (bytes_read > 0) {
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if (bytes_read == -1 && errno == EINTR) {
                        // 客户端正常中断，继续读取
                        printf("continue reading\n");
                        continue;
                    } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                        // bytes_read == -1说明已经没有数据了，加上后面两个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (bytes_read == 0) { // EOF, 客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd); // 关闭socket会自动将文件描述符从树上移除
                        break;
                    }
                }
            } else { // 其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }

    close(sockfd);
    return 0;
}
