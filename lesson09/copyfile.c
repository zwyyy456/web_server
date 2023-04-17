#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 通过open打开一个文件
    int srcfd = open("english.txt", O_RDONLY);
    if (srcfd == -1) {
        perror("open");
        return -1;
    }
    // 创建一个新的文件（拷贝的目标）
    int desfd = open("copy.txt", O_RDWR | O_CREAT, 0664);
    if (desfd == -1) {
        perror("open");
        return -1;
    }

    // 频繁的读写操作
    char buf[1024] = {0};
    int read_len = 0;
    while ((read_len = read(srcfd, buf, sizeof(buf))) > 0) {
        // printf("%c ", buf[0]);
        write(desfd, buf, read_len);
    }

    // 关闭文件
    close(desfd);
    close(srcfd);
    return 0;
}