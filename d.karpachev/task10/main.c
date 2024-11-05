#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
  pid_t pid;

  if (argc < 2) {
    perror("Arguments missing\n");
    return 1;
  }

  pid = fork();

  if (pid == 0) {
    execvp(argv[1], &argv[1]);
    perror("execvp failed");
    return 1;
  } 
  else if (pid > 0) {
    printf("Subprocess {%s} started.\n", argv[1]);

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      printf("\nSubprocess finished with code {%d}.\n", WEXITSTATUS(status));
    }
    else {
      perror("\nSubprocess finished abnormal.\n");
    }
  } 
  else {
    perror("fork failed");
    return 1;
  }

  return 0;
}

