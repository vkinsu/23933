#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
  pid_t pid;

  if (argc < 2) {
    perror("File name missing\n");
    return 1;
  }
  if (argc > 2) {
    perror("Too many arguments.\n");
    return 1;
  }

  pid = fork();

  if (pid == 0) {
    char *my_args[] = {"cat", argv[1], NULL};
    execvp("cat", my_args);
    perror("execvp failed");
    return 1;
  } 
  else if (pid > 0) {
    printf("Родительский процесс: приветствие\n");

    waitpid(pid, NULL, 0);

    printf("\nРодительский процесс: подпроцесс завершен\n");
  } 
  else {
    perror("fork failed");
    return 1;
  }

  return 0;
}

