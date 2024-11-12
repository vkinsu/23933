#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", "text.txt", NULL);
        perror("execlp failed");
        return 1;
    } else {
        printf("Start of the parent process\n");

        if (waitpid(pid, NULL, 0) != -1) {
            printf("Child process (pid: %d) has finished\n", pid);
        } else {
            perror("waitpid failed");
        }
    }

    return 0;
}
