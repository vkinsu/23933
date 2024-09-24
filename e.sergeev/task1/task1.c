#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

void print_usage(char* program_name) {
    fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-Unew_ulimit] [-c] [-Csize] [-d] [-v] [-Vname=value] ...n", program_name);
    exit(1);
}

int main(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "ispucC:d:vV:U:")) != -1) {
        switch (opt) {
        case 'i':
            printf("Real UID: %dn", getuid());
            printf("Effective UID: %dn", geteuid());
            printf("Real GID: %dn", getgid());
            printf("Effective GID: %dn", getegid());
            break;
        case 's':
            if (setpgid(0, 0) == -1) {
                perror("setpgid");
                exit(1);
            }
            break;
        case 'p':
            printf("Process ID: %dn", getpid());
            printf("Parent process ID: %dn", getppid());
            printf("Process group ID: %dn", getpgid(0));
            break;
        case 'u': {
            struct rlimit rlim;
            if (getrlimit(RLIMIT_AS, &rlim) == -1) {
                perror("getrlimit");
                exit(1);
            }
            printf("ulimit: %ldn", rlim.rlim_cur);
            break;
        }
        case 'U': {
            long new_ulimit = atol(optarg);
            if (new_ulimit == 0 && errno == ERANGE) {
                fprintf(stderr, "Invalid ulimit value: %sn", optarg);
                exit(1);
            }
            struct rlimit rlim;
            rlim.rlim_cur = new_ulimit;
            rlim.rlim_max = new_ulimit;
            if (setrlimit(RLIMIT_AS, &rlim) == -1) {
                perror("setrlimit");
                exit(1);
            }
            break;
        }
        case 'c': {
            struct rlimit rlim;
            if (getrlimit(RLIMIT_CORE, &rlim) == -1) {
                perror("getrlimit");
                exit(1);
            }
            printf("Core file size limit: %ldn", rlim.rlim_cur);
            break;
        }
        case 'C': {
            long size = atol(optarg);
            if (size == 0 && errno == ERANGE) {
                fprintf(stderr, "Invalid core file size: %sn", optarg);
                exit(1);
            }
            struct rlimit rlim;
            rlim.rlim_cur = size;
            rlim.rlim_max = size;
            if (setrlimit(RLIMIT_CORE, &rlim) == -1) {
                perror("setrlimit");
                exit(1);
            }
            break;
        }
        case 'd':
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                perror("getcwd");
                exit(1);
            }
            printf("Current working directory: %sn", cwd);
            break;
        case 'v': {
            char* env;
            for (env = environ; *env != NULL; env++) {
                printf("%sn", *env);
            }
            break;
        }
        case 'V': {
            char* name = strtok(optarg, "=");
            char* value = strtok(NULL, "=");
            if (value == NULL) {
                fprintf(stderr, "Invalid environment variable: %sn", optarg);
                exit(1);
            }
            if (setenv(name, value, 1) == -1) {
                perror("setenv");
                exit(1);
            }
            break;
        }
        default:
            print_usage(argv[0]);
        }
    }
    return 0;
}
