#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/epoll.h>
#include <csignal>

#include "locker.h"
#include "threadpool.h"

// 添加信号捕捉
void AddSig(int sig, void(handler)(int)) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, nullptr);
}

auto main(int argc, char *argv[]) -> int {
    if (argc <= 1) {
        printf("run as follows: %s port_number\n", basename(argv[0]));
        exit(-1);
    }

    // 获取端口号
    int port = atoi(argv[1]);
    AddSig(SIGPIPE, SIG_IGN);

    // 创建线程池，初始化线程池
    // 这里我们认为是http连接任务
    Threadpool<HttpConn> *pool = nullptr;
    try {
        pool = new Threadpool<HttpConn>;
    } catch (...) {
        exit(-1);
    }
    printf("str");
    return 0;
}