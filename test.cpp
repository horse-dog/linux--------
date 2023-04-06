#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd, newfd;
    int flags;

    // 创建一个文件描述符
    fd = open("test.txt", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 设置close-on-exec和non-blocking属性
    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }
    flags |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, flags) == -1) {
        perror("fcntl");
        return 1;
    }

    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return 1;
    }

    // 复制文件描述符
    newfd = dup(fd);
    if (newfd == -1) {
        perror("dup");
        return 1;
    }

    // 检查新的文件描述符是否继承了close-on-exec和non-blocking属性
    flags = fcntl(newfd, F_GETFD);
    if (flags & FD_CLOEXEC)
        printf("newfd inherited close-on-exec flag\n");
    else
        printf("newfd did not inherit close-on-exec flag\n");

    flags = fcntl(newfd, F_GETFL);
    if (flags & O_NONBLOCK)
        printf("newfd inherited non-blocking flag\n");
    else
        printf("newfd did not inherit non-blocking flag\n");

    close(fd);
    close(newfd);

    return 0;
}