#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        system("cat file.txt");
    } else {
        waitpid(pid, NULL, 0);

        int fd = open("text.txt", O_RDONLY);
        size_t len = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        char *msg = malloc(len + 1);
        read(fd, msg, len);
        msg[len] = 0;
        printf("\n%s\n", msg);

        free(msg);
        close(fd);
    }
}