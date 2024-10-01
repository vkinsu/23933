#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <errno.h>

void print_ulimit();
void set_ulimit(long new_limit);
void print_core_size();
void set_core_size(long size);
void print_working_directory();
void print_env();
void set_env(char* name, char* value);

int main(int argc, char* argv[]) {

  if (argc <= 1) {
    perror("No argements here!\n");
    return EXIT_FAILURE;
  }

  char cmd = 0;
  char* name = NULL;
  char* eq = NULL;
  while ((cmd = getopt(argc, argv, "ispucdvU:V:C:")) != -1) {
    switch (cmd) {
      case 'i':
        printf("Real UID: %d\nEffective UID: %d\n", getuid(), geteuid());
        printf("Real GID: %d\nEffective GID: %d\n\n", getgid(), getegid());
        break;
      case 's':
        setpgid(0, 0);
        printf("Process is leader now.\n\n");
        break;
      case 'p':
        printf("PID: %d\nPPID: %d\nPGID: %d\n\n", getpid(), getppid(), getpgrp());
        break;
      case 'u':
        print_ulimit();
        break;
      case 'U':
        set_ulimit(strtol(optarg, NULL, 10));
        break;
      case 'c':
        print_core_size();
        break;
      case 'C':
        set_core_size(strtol(optarg, NULL, 10));
        break;
      case 'd':
        print_working_directory();
        break;
      case 'v':
        print_env();
        break;
      case 'V':
        name = strdup(optarg);
        eq = strchr(name, '=');
        if (eq) {
          *eq = '\0';
          set_env(name, eq + 1);
        }
        free(name);
        break;
      default:
        perror("Invalid option\n");
        break;
    }
  }

  return 0;
}

void print_ulimit(){
  struct rlimit rlim;
  if (getrlimit(RLIMIT_FSIZE, &rlim) == 0)
    printf("Ulimit: %lu\n\n", rlim.rlim_cur);
  else
    perror("Get ulimit error\n");
}

void set_ulimit(long new_limit) {
  struct rlimit rlim;
  rlim.rlim_cur = new_limit;
  rlim.rlim_max = new_limit;
  if (new_limit < 0) {
    perror("Wrong ulimit value\n");
    return;
  }
  if (setrlimit(RLIMIT_FSIZE, &rlim) == 0)
    printf("Ulimit succesfully changed\n\n");
  else
    perror("Set ulimit error\n");
}

void print_core_size(){
  struct rlimit rlim;
  if (getrlimit(RLIMIT_CORE, &rlim) == 0)
    printf("Core limit: %lu\n\n", rlim.rlim_cur);
  else
    perror("Get core size error\n");
}

void set_core_size(long size){
  struct rlimit rlim;
  rlim.rlim_cur = size;
  rlim.rlim_max = size;
  if (setrlimit(RLIMIT_CORE, &rlim) == 0)
    printf("Core size succesfully changed\n\n");
  else
    perror("Incorrect size\n");
}

void print_working_directory() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("Working directory: %s\n\n", cwd);
  else
    perror("Finding directory error\n");
}

void print_env() {
  extern char **environ;
  printf("Environment variables:\n");
  for (char **env = environ; *env != 0; env++) {
      printf("\t%s\n", *env);
  }
  printf("\n");
}

void set_env(char* name, char* value) {
  if (setenv(name, value, 1) == 0)
    printf("New variable %s with value %s was setted.\n\n", name, value);
  else
    perror("Error at variable setter.\n");
}
