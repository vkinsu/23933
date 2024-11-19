#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        ssize_t numRead;
        while ((numRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            for (ssize_t i = 0; i < numRead; ++i) {
                buffer[i] = toupper(buffer[i]);
            }
            write(STDOUT_FILENO, buffer, numRead);
        }

        close(pipefd[0]); 
        _exit(EXIT_SUCCESS);

    } else {
        close(pipefd[0]);

        const char* text = "Hello World\nThis is a Test\n";
        write(pipefd[1], text, strlen(text));

        close(pipefd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
