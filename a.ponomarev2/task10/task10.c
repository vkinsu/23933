#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Argument error\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error process creation");
        return 1;

    } else if (pid == 0) {

        execvp(argv[1], &argv[1]);

        perror("execvp");
        return 1;

    } else {
        printf("Parent process: waiting\n");
        int status;

        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {

            int exit_status = WEXITSTATUS(status);
            printf("\nThe child process is completed with the code: %d\n", exit_status);
        } else {
            printf("\nThe child process terminated with an error\n");
        }
    }

    return 0;
}
