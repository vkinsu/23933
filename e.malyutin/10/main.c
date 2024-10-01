#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s [cmd] [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        if (execvp(argv[1], argv + 1) == -1) {
            perror("exec");
            exit(1);
        }
    } else {
        int status;
        if (wait(&status) == -1) {
            perror("wait");
            return 1;
        }
        printf("exit code: %d\n", WEXITSTATUS(status));
    }
}