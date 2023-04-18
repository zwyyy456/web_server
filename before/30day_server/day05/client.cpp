#include "util.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFFER_SIZE 2048
int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(sockfd == -1, "socket create error\n");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 将IP地址的文本表示方式转换为符合套接字地址要求的格式
    serv_addr.sin_port = htons(8886);

    // bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)); // 客户端不进行bind操作
    ErrIf(connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error\n");
    while (true) {
        char buf[BUFFER_SIZE];
        bzero(&buf, sizeof(buf));
        printf("please input text:\n");
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf, sizeof(buf)); // 清空，读服务器发过来的数据
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from server: %s\n", buf);
        } else if (read_bytes == 0) {
            printf("server socket disconnected!\n");
            break;
        } else if (read_bytes == -1) {
            close(sockfd);
            ErrIf(true, "socket read error\n");
        }
    }
    close(sockfd);
    return 0;
}