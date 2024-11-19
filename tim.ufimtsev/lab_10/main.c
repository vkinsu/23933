#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not exit normally\n");
        }
    }

    return 0;
}
