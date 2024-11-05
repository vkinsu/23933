#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


int main() {
    // -> write() p[1] -> -> read() p[0] ->
    // pipe - file inside

    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid;
    pid = fork();

    char buffer[256];

    if (pid > 0) {  // parent
        close(pipe_fd[0]);

        char *text = "Hello World!";
        write(pipe_fd[1], text, strlen(text)+1);

        close(pipe_fd[1]);

        wait(NULL); // wait for child

    } else {  // child
        close(pipe_fd[1]);

        // read text
        ssize_t bytes_read = read(pipe_fd[0], buffer, 256);
        buffer[bytes_read] = '\0';

        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = (char )toupper(buffer[i]);
        }

        printf("received: %s\n", buffer);
        close(pipe_fd[0]);
    }

    return 0;
}
