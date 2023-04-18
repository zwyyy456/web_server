#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 创建一个新的文件
    int fd = open("create.txt", O_RDWR | O_CREAT, 0777);
    if (fd == -1) {
        perror("open");
    }
    // 关闭
    close(fd);
    return 0;
}