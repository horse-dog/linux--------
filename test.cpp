#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int test_dup() {
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

#include <signal.h>

void handler(int x) {
    printf("sigusr1...\n");
}

void test_signal() {

    signal(SIGUSR1, handler);
    raise(SIGUSR1);
    raise(SIGUSR1);

}

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

void mmap_shared() {
    void* shared_memory = mmap64(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid_t pid = fork();

    if (pid == 0) {
        printf("child: shared memory contains: %s\n", (char*)shared_memory);
        strcpy((char*)shared_memory, "Hello from child process!");
        printf("child: shared memory now contains: %s\n", (char*)shared_memory);
    } else {
        strcpy((char*)shared_memory, "Hello from parent process!");
        printf("parent: shared memory contains: %s\n", (char*)shared_memory);
        wait(NULL);
        printf("parent: shared memory now contains: %s\n", (char*)shared_memory);
    }

    munmap(shared_memory, 4096);
}

void lhandler(int signo) {
    printf("sigusr1 start...\n");
    int i, j, k;
    volatile long long x;
    int n = 2000;
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            for (k = 1; k < n; k++)
                x += (i * j / k);
    printf("sigusr1 finished...\n");
}

void test_sigmask() {

    printf("%d\n", getpid());
    sleep(10);
    printf("signal sigusr1\n");
    signal(SIGUSR1, lhandler);
    printf("finish signal sigusr1\n");

    while (1);

}

void test_sigaction_mask() {

    printf("%d\n", getpid());
    sleep(10);
    printf("signal sigusr1\n");
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = lhandler;
    act.sa_flags = 0;
    act.sa_flags |= SA_NOMASK;    
    sigaction(SIGUSR1, &act, NULL);
    printf("finish signal sigusr1\n");

    while (1);

}

int main() {

    // test_signal();
    // mmap_shared();
    // test_sigmask();
    test_sigaction_mask();
    return 0;

}