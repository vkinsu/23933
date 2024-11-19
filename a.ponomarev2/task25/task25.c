#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Argument error");
        return 1;
    }

    int fd[2]; //fd[0] - чтение, fd[1] - запись
    pid_t pid;
    
    // Создаем канал
    if (pipe(fd) == -1) {
        perror("pipe error");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork error");
        return 1;
    }

    if (pid == 0) {
        close(fd[1]);
        char buffer[BUFFER_SIZE];

        ssize_t n;
        while ((n = read(fd[0], buffer, BUFFER_SIZE)) > 0) {

            for (ssize_t i = 0; i < n; i++) {
                buffer[i] = toupper(buffer[i]);
            }

            write(STDOUT_FILENO, buffer, n);
        }

        close(fd[0]);
        exit(0);
    } else {
        
        close(fd[0]); 

        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        size_t bytes_read;

        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            write(fd[1], buffer, bytes_read);
        }

        fclose(file);
        close(fd[1]); 

        waitpid(pid, NULL, 0);
    }

    return 0;
}
