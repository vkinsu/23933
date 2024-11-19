#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error process creation");
        return 1;

    } else if (pid == 0) {
        execlp("cat", "cat", "file.txt", NULL);
        perror("execlp");
        return 1;

    } else {
        printf("Parent process: waiting\n");
        
        waitpid(pid, NULL, 0);
        
        printf("\nThe child process is complete\n");
    }

    return 0;
}
