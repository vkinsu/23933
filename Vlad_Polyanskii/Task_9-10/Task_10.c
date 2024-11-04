#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc < 2){
        printf("Wrong number of parametres\n");
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("Can't create child process");
        return -1;
    }

    if(pid == 0){
        if(execvp(argv[1], argv + 1) == -1){
            perror("execvp(argv[1], argv + 1) return -1");
            return -1;
        }
    }
    else{
        int stat;
        wait(&stat);
        printf("\nchild process has returned: %d\n", stat);
    }
    
    return 0;
}