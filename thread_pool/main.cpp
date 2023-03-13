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

    return 0;
}