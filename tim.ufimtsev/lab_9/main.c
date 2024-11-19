#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        execlp("cat", "cat", "long_file.txt", (char *)NULL);
        perror("execlp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Process exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("Process terminated abnormally\n");
        }
    }

    return 0;
}
