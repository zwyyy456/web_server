#include "util.h"

#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <sys/socket.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4/ipv6，标识流格式、面向连接，协议0表示自动推导
    ErrIf(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    ErrIf(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    ErrIf(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    // 保存连接的客户端的socket地址信息
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, client_addr_len);

    int client_sockfd = accept(sockfd, (sockaddr *)&client_addr, &client_addr_len);
    printf("new client fd %d :: IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return 0;
}