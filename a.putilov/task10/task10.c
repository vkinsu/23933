#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error creating process");
        return 1;
    } 
    else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("Error executing command");
        exit(1);
    } 
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) printf("Exit status of the command '%s': %d\n", argv[1], WEXITSTATUS(status));
        else printf("The command '%s' did not exit normally.\n", argv[1]);
    }

    return 0;
}

