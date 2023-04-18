#include "inc/buffer.h"
#include "inc/socket.h"
#include "inc/util.h"
#include "inc/inet_address.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFER_SIZE 2048
int main() {
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8886);
    sock->Connect(addr);

    int sockfd = sock->get_fd();

    Buffer *send_buffer = new Buffer();
    Buffer *read_buffer = new Buffer();

    while (true) {
        send_buffer->getline();
        ssize_t write_bytes = write(sockfd, send_buffer->c_str(), send_buffer->size());
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024]; // 这里这个buf大小已经无所谓了，有buffer
        while (true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if (read_bytes > 0) {
                read_buffer->Append(buf, read_bytes);
                already_read += read_bytes;
            } else if (read_bytes == 0) {
                // EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if (already_read >= send_buffer->size()) {
                printf("message from server: %s\n", read_buffer->c_str());
                break;
            }
        }
        read_buffer->Clear();
    }
    delete addr;
    delete sock;
    return 0;
}