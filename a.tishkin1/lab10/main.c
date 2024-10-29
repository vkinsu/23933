#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
    if (argc < 2){
        perror("missing command name argument\n");
        return 1;
    }

    pid_t child = fork();

    if (child == -1) {
        perror("failed to create child process\n");
        return 1;
    }

    if (child == 0) {
//        printf("Child process is running\n");

        execvp(argv[1], &argv[1]);

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

            printf("Child process exit with code: %d\n", exit_code);
        } else {
            printf("Child process didnt complete normally\n");
        }

//        printf("Parent process ended\n");
    }

    return 0;
}
