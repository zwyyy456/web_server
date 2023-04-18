#include "../inc/connection.h"
#include "../inc/socket.h"
#include "../inc/buffer.h"
#include "../inc/channel.h"
#include "../inc/util.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *loop, Socket *sock) :
    loop_(loop), sock_(sock), channel_(nullptr), in_buffer_(new std::string()), read_buffer_(nullptr) {
    channel_ = new Channel(loop_, sock_->get_fd());
    std::function<void()> callback = [this, capture0 = sock_->get_fd()] { Echo(capture0); };
    channel_->set_callback_(callback);
    channel_->EnableReading();

    read_buffer_ = new Buffer();
}

Connection::~Connection() {
    delete channel_;
    delete sock_;
}

void Connection::Echo(int sockfd) {
    char buf[READ_BUFFER]; // 此时，这个buf大小已经无所谓了
    while (true) {
        // 非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            // printf("message from client fd %d: %s\n", sockfd, buf);
            read_buffer_->Append(buf, bytes_read);
            // write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            // 客户端正常中断，继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            printf("message from client fd %d: %s\n", sockfd, read_buffer_->c_str());
            ErrIf(write(sockfd, read_buffer_->c_str(), read_buffer_->size()) == -1, "socket write error");
            break;
        } else if (bytes_read == 0) {
            // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // 关闭socket会自动将文件描述符从epoll树上移除
            delete_connection_callback_(sock_);
            break;
        }
    }
}

void Connection::set_delete_connection_callback_(std::function<void(Socket *)> callback) {
    delete_connection_callback_ = callback;
}