#include "http_conn.h"
#include "locker.h"
#include "thread_pool.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_FD 65535
#define MAX_EVENT_NUM 10000 // 一次监听的最大数量
// 添加信号捕捉
void AddSignal(int sig, void(handler(int))) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask); // 临时都设置成阻塞
    sigaction(sig, &sa, nullptr);
}

int main(int argc, char *argv[]) {
    if (argc <= 1) { // 至少要传递端口数, argv[0]为程序名
        printf("please run as follow: %s port_nubmer\n", basename(argv[0]));
        exit(-1);
    }

    // 获取端口号
    int port = atoi(argv[1]);

    // 对SIGPIE信号进行处理
    AddSignal(SIGPIPE, SIG_IGN);

    // 创建线程池，初始化线程池
    ThreadPool<HttpConn> *pool = nullptr;
    try {
        pool = new ThreadPool<HttpConn>;
    } catch (...) {
        exit(-1);
    }

    // 创建一个数组用于保存所有的客户端信息
    HttpConn *user = new HttpConn[MAX_FD];

    // 写网络代码
    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

    // 设置端口复用
    int reuse = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(listen_fd, (struct sockaddr *)&address, sizeof(address));

    // 监听
    listen(listen_fd, 5);

    // 创建epoll对象，事件数组，添加监听的文件描述符
    epoll_event events[MAX_EVENT_NUM];

    int epoll_fd = epoll_create(5);

    // 将监听的描述符添加到epoll对象中
    add_fd(epoll_fd, listen_fd, false);
    HttpConn::epoll_fd_ = epoll_fd;

    while (1) {
        int num = epoll_wait(epoll_fd, events, MAX_EVENT_NUM, -1);
        if ((num < 0) && (errno != EINTR)) {
            printf("epoll failure\n");
            break;
        }
        // 循环遍历事件数组
        for (int i = 0; i < num; ++i) {
            int sock_fd = events[i].data.fd;
            if (sock_fd == listen_fd) {
                // 说明有客户端连接进来
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(client_address);
                int conn_fd = accept(listen_fd, (struct sockaddr *)&client_address, &client_addrlen);
                if (HttpConn::user_cnt_ >= MAX_FD) {
                    // 目前连接数满了，服务器正忙
                    // 给客户端写一个信息：服务器正忙
                    close(conn_fd);
                    continue;
                }
            }
        }
    }

    return 0;
}