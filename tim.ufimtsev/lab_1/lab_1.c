#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

void print_ids() {
    printf("User ID: %d, Effective User ID: %d, Group ID: %d, Effective Group ID: %d\n",
           getuid(), geteuid(), getgid(), getegid());
}

void set_process_group() {
    if (setpgid(0, 0) == -1) {
        perror("Failed to set process group leader\n");
    } else {
        printf("Process group leader set successfully\n");
    }
}

void print_process_info() {
    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n",
           getpid(), getppid(), getpgrp());
}

void print_ulimit() {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_FSIZE, &rlim) == -1) {
        perror("Failed to get file size limit\n");
    } else {
        printf("File size limit: %llu\n", rlim.rlim_max);
    }
}

void set_ulimit(char *optarg) {
    long new_limit = strtol(optarg, NULL, 10);

    if (new_limit == 0) {
        perror("Invalid argument for -U option\n");
        return;
    }

    struct rlimit rlim;
    if (getrlimit(RLIMIT_FSIZE, &rlim) == -1) {
        perror("Failed to get file size limit\n");
        return;
    }
    rlim.rlim_cur = new_limit;
    if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
        perror("Failed to set file size limit\n");
    } else {
        printf("File size limit set successfully\n");
    }
}

void print_core_limit() {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_CORE, &rlim) == -1) {
        perror("Failed to get core file size limit\n");
    } else {
        printf("Core file size limit: %llu\n", rlim.rlim_max);
    }
}

void set_core_limit(char *optarg) {
    long long new_core_limit = strtoll(optarg, NULL, 10);

    if (new_core_limit == 0) {
        perror("Invalid argument for -C option\n");
        return;
    }

    struct rlimit rlim;
    if (getrlimit(RLIMIT_CORE, &rlim) == -1) {
        perror("Failed to get core file size limit\n");
        return;
    }
    rlim.rlim_cur = new_core_limit;
    if (setrlimit(RLIMIT_CORE, &rlim) == -1) {
        perror("Failed to set core file size limit\n");
    } else {
        printf("Core file size limit set successfully\n");
    }
}

void print_current_directory() {
    char *current_dir = getenv("PWD");

    if (current_dir == NULL) {
        perror("Failed to get current directory\n");
    } else {
        printf("Current directory: %s\n", current_dir);
    }
}

void print_environment() {
    extern char **environ;
    char **env = environ;
    for (; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

void set_environment_variable(char *optarg) {
    if (putenv(optarg) == -1) {
        printf("Failed to set environment variable\n");
    }
}

int main(int argc, char *argv[]) {

    char opts[] = ":ispuU:cC:dvV:"; 
    int opt;

    while ((opt = getopt(argc, argv, opts)) != -1) {
        
        switch (opt) {
            case 'i': print_ids(); break;
            case 's': set_process_group(); break;
            case 'p': print_process_info(); break;
            case 'u': print_ulimit(); break;
            case 'U': set_ulimit(optarg); break;
            case 'c': print_core_limit(); break;
            case 'C': set_core_limit(optarg); break;
            case 'd': print_current_directory(); break;
            case 'v': print_environment(); break;
            case 'V': set_environment_variable(optarg); break;
            default: printf("Invalid option: %c\n", optopt);
        }
    }

    return 0;
}
