#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    int fd[2];
    if(pipe(fd) != 0){
        perror("Can't do pipe:");
        return 1;
    }
    
    pid_t pid = fork();
    if(pid == -1){
        perror("Can't create child process");
        return 1;
    }

    if(pid == 0){
        close(fd[1]);
        char c;
        char mode = 'r';
        FILE* stream = fdopen(fd[0], &mode);
        if(stream == NULL){
            perror("Can't open parent stream:");
            return 1;
        }

        c = fgetc(stream);
        while(c != EOF){
            c = toupper(c);
            putchar(c);
            c = fgetc(stream);
        }

        fclose(stream);
        return 0;
    }
    else{
        close(fd[0]);
        char mode = 'w';
        FILE* stream = fdopen(fd[1], &mode);
        if(stream == NULL){
            perror("Can't open parent stream:");
            return 1;
        }

        fprintf(stream, "jhhhjjjKKKKjjHHHHlkdk");
        fclose(stream);
        
        int stat;
        wait(NULL);
        return 0;
    }
}