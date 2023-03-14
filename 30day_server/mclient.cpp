#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <sys/socket.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = htons(8888);

    // bind(sockfd, (sockaddr *)&client_addr, sizeof(client_addr)); // 客户端不进行bind操作
    connect(sockfd, (sockaddr *)&client_addr, sizeof(client_addr));
    return 0;
}