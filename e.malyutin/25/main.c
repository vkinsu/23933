#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define lenof(x) sizeof(x) / sizeof(*x)

char *msgs[] = {
    "heLLo",
    "i'M cHiLD",
    "goOdbye",
};

int main(void) {
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(fd[0]);
        for (int i = 0; i < lenof(msgs); i++) {
            write(fd[1], msgs[i], strlen(msgs[i]) + 1);
            write(fd[1], "\n", 1);
            sleep(1);
        }
        close(fd[1]);
    } else {
        close(fd[1]);
        char buf[128];
        int size;
        while ((size = read(fd[0], buf, sizeof(buf))) > 0) {
            for (int i = 0; i < size; i++) {
                buf[i] = toupper(buf[i]);
            }
            write(STDOUT_FILENO, buf, size);
        }
        wait(NULL);
    }
}