#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define PIPE_SIZE 1024

int main() {
  int pipefd[2];
  pid_t pid;
  char buffer[PIPE_SIZE];
  int bytes_read;

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    close(pipefd[1]);

    while ((bytes_read = read(pipefd[0], buffer, PIPE_SIZE)) > 0) {
      for (int i = 0; i < bytes_read; i++) {
        if (97 <= (int)buffer[i] <= 122) {
          buffer[i] = toupper(buffer[i]);
        }
      }
      write(STDOUT_FILENO, buffer, bytes_read);
    }

    close(pipefd[0]);
    exit(0);
  } else {
    close(pipefd[0]);

    bytes_read = read(STDIN_FILENO, buffer, PIPE_SIZE);

    write(pipefd[1], buffer, bytes_read);

    close(pipefd[1]);
    wait(NULL);
  }

  return 0;
}

