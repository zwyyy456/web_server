#include "http_conn.h"

// 所有的客户数，静态变量要初始化
int HttpConn::user_cnt_ = 0;
int HttpConn::epoll_fd_ = -1;

// 设置文件描述符非阻塞
void SetNonBlocking(int fd) {
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
}

// 添加文件描述符到epoll中，
void add_fd(int epoll_fd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    // event.events = EPOLLIN | EPOLLRDHUP;
    // 边沿触发
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;

    if (one_shot) {
        // 防止同一个通信被不同的线程处理
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    // 设置文件描述符非阻塞
    SetNonBlocking(fd);
}

// 从epoll中移除监听的文件描述符
void rm_fd(int epoll_fd, int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

// 重置socket上EPOLLONESHOT事件，以确保下一次可读时， EPOLLIN事件能被触发
void mod_fd(int epoll_fd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

// 关闭连接
void HttpConn::Init(int sock_fd, const sockaddr_in &addr) {
    sock_fd_ = sock_fd;
    address_ = addr;

    // 设置端口复用
    int reuse = 1;
    setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 添加到epoll对象中
    add_fd(epoll_fd_, sock_fd_, true);
    user_cnt_++; // 总用户数+1

    Init();
}

void HttpConn::Init() {
    check_state_ = CHECK_STATE_REQUESTLINE; // 初始化状态为解析请求首行
    checked_idx_ = 0;
    start_idx_line_ = 0;
    read_idx_ = 0;
}

// 初始化连接
void HttpConn::CloseConn() {
    if (sock_fd_ != -1) {
        rm_fd(epoll_fd_, sock_fd_);
        --user_cnt_; // 关闭一个连接，客户连接数要减一
    }
}

// 主状态机，解析请求
HttpConn::HTTP_CODE HttpConn::ProcessRead() {
    // 解析HTTP请求
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;

    char *text = 0;
    while ((check_state_ == CHECK_STATE_CONTENT && line_status_ == LINE_OK) || (line_status = ParseLine()) == LINE_OK) {
        // 解析到了一行完整的数据，或者解析到了请求体，也是一行完整的数据
        text = GetLine();
        start_idx_line_ = checked_idx_;
        printf("got 1 http line:\n%s\n", text);

        switch (check_state_) {
        case CHECK_STATE_REQUESTLINE: {
        }

        case CHECK_STATE_HEADER: {
        }
        }
    }
    return NO_REQUEST;
}

HttpConn::ParseRequestLine(char *text);       // 解析请求首行
HTTP_CODE HttpConn::ParseHeaders(char *text); // 解析请求头
HTTP_CODE HttpConn::ParseContent(char *text); // 解析请求内容

HttpConn::LINE_STATUS HttpConn::ParseLine() {
    return LINE_OK;
}

// 由线程池中的工作线程调用，是处理HTTP请求的入口函数
void HttpConn::Process() {
    // 解析HTTP请求
    HTTP_CODE read_ret = ProcessRead();
    if (read_ret == NO_REQUEST) {
        mod_fd(epoll_fd_, sock_fd_, EPOLLIN);
        return;
    }
    printf("parse request, create response\n");

    // 生成响应
}

// 循环读取客户数据，直到无数据可读或者对方关闭连接
bool HttpConn::Read() {
    if (read_idx_ >= kread_buf_size) {
        // 缓冲区已经满了，等待下一次再读
        return false;
    }

    // 读取到的字节
    int bytes_read = 0;
    while (1) {
        bytes_read = recv(sock_fd_, read_buf_ + read_idx_, kread_buf_size - read_idx_, 0);
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 没有数据了，读完了
                break;
            } else {
                return false;
            }
        } else if (bytes_read == 0) {
            // 对方已关闭连接
            return false;
        }
        read_idx_ += bytes_read;
    }
    // printf("read all data once\n");
    printf("read data:\n%s\n", read_buf_);
    return true;
}

bool HttpConn::Write() {
    printf("write all onece\n");
    return true;
}