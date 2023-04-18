#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include "locker.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class HttpConn {
  public:
    static int epoll_fd_;                    // 所有socket上的事件都被注册到同一个epoll对象中
    static int user_cnt_;                    // 统计用户数量
    static const int kread_buf_size = 2048;  // 读缓冲区的大小
    static const int kwrite_buf_size = 1024; // 写缓冲区的大小

    // HTTP请求方法，这里只支持GET
    enum METHOD { GET = 0,
                  POST,
                  HEAD,
                  PUT,
                  DELETE,
                  TRACE,
                  OPTIONS,
                  CONNECT };

    /*
        解析客户端请求时，主状态机的状态
        CHECK_STATE_REQUESTLINE:当前正在分析请求行
        CHECK_STATE_HEADER:当前正在分析头部字段
        CHECK_STATE_CONTENT:当前正在解析请求体
    */
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0,
                       CHECK_STATE_HEADER,
                       CHECK_STATE_CONTENT };

    /*
        服务器处理HTTP请求的可能结果，报文解析的结果
        NO_REQUEST          :   请求不完整，需要继续读取客户数据
        GET_REQUEST         :   表示获得了一个完成的客户请求
        BAD_REQUEST         :   表示客户请求语法错误
        NO_RESOURCE         :   表示服务器没有资源
        FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
        FILE_REQUEST        :   文件请求,获取文件成功
        INTERNAL_ERROR      :   表示服务器内部错误
        CLOSED_CONNECTION   :   表示客户端已经关闭连接了
    */
    enum HTTP_CODE { NO_REQUEST,
                     GET_REQUEST,
                     BAD_REQUEST,
                     NO_RESOURCE,
                     FORBIDDEN_REQUEST,
                     FILE_REQUEST,
                     INTERNAL_ERROR,
                     CLOSED_CONNECTION };

    // 从状态机的三种可能状态，即行的读取状态，分别表示
    // 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
    enum LINE_STATUS { LINE_OK = 0,
                       LINE_BAD,
                       LINE_OPEN };
    // HttpConn();
    // ~HttpConn();

    void Process(); // 处理客户端请求

    void Init(int sock_fd, const sockaddr_in &addr); // 初始化新接受的连接
    void CloseConn();                                // 关闭连接

    bool Read();
    bool Write();

  private:
    int sock_fd_;                   // 该http连接的socket
    sockaddr_in address_;           // 通信的socket地址
    char read_buf_[kread_buf_size]; // 读缓冲区
    int read_idx_;                  // 标识读缓冲区中已经读入的客户端数据的最后一个字节的下一个位置
    int checked_idx_;               // 当前正在分析的字符在读缓冲区的位置
    int start_idx_line_;            // 当前在解析的行的起始位置
    CHECK_STATE check_state_;       // 主状态机当前所处的状态
    LINE_STATUS line_status_;

    void Init(); // 初始化连接其余的信息

    HTTP_CODE ProcessRead();                // 解析HTTP请求
    HTTP_CODE ParseRequestLine(char *text); // 解析请求首行
    HTTP_CODE ParseHeaders(char *text);     // 解析请求头
    HTTP_CODE ParseContent(char *text);     // 解析请求内容

    LINE_STATUS ParseLine(); // 解析一行

    char *GetLine() { return read_buf_ + start_idx_line_; }
};

// 添加文件描述符到epoll中，
void add_fd(int epoll_fd, int fd, bool one_shot);
// 从epoll中删除文件描述符
void rm_fd(int epoll_fd, int fd);

void mod_fd(int epoll_fd, int fd, int ev);

void SetNonBlocking(int fd);

#endif