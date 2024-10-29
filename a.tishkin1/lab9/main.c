#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
    if (argc < 2){
        perror("missing filename argument\n");
        return 1;
    }

    pid_t child = fork();

    if (child == -1) {
        perror("failed to create child process\n");
        return 1;
    }

    if (child == 0) {
//        printf("Child process is running\n");

        execlp("cat", "cat", argv[1], NULL);

        return 0;
    } else {
//        printf("Parent process is running\n");

        int status;
        pid_t wp = waitpid(child, &status, 0);

        if (wp == -1){
            perror("failed to waitid");
            return 1;
        }

        if (WIFEXITED(status)){
            int exit_code = WEXITSTATUS(status);

            if (exit_code == 0) {
                printf("Child process completed successfully\n");
            }
            if (exit_code == 1) {
                printf("Child process exit with error code: %d\n", exit_code);
            }

        } else {
            printf("Child process didnt complete normally\n");
        }

//        printf("Parent process ended\n");
    }

    return 0;
}