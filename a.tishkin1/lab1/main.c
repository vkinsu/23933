#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdlib.h>



extern char **environ;


int main(int argc, char *argv[]) {
    char options[] = "i,s,p,u,c,d,U:,C:,v,V:";  /* valid options */
    int c;

    struct rlimit data;
    char path[100];

    if (argc == 1) { perror("run without arguments"); return 0;}

    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            case 'i':
                printf("Effective user id: %u.\nEffective group id: %u.\n", geteuid(), getegid());
                printf("Real user id: %u.\nReal group id: %u.\n", getuid(), getgid());
                break;
            case 's':
                setpgid(0, 0);  // process becomes a process group leader
                break;
            case 'p':
                printf("PID: %u.\n", getpid());
                printf("Parent PID: %u.\n", getppid());
                printf("Group ID: %u.\n", getpgrp());
                break;
            case 'u':
                getrlimit(RLIMIT_NOFILE, &data);
                printf("Ulimit value. Soft limit: %llu. Hard limit: %llu.\n", data.rlim_cur, data.rlim_max);
                break;
            case 'U':
                getrlimit(RLIMIT_NOFILE, &data);
                data.rlim_cur = (rlim_t) atoi(optarg);
                setrlimit(RLIMIT_NOFILE, &data);
                break;
            case 'c':
                getrlimit(RLIMIT_CORE, &data);
                printf("Core values. Soft limit: %llu. Hard limit: %llu.\n", data.rlim_cur, data.rlim_max);
                break;
            case 'C':
                getrlimit(RLIMIT_CORE, &data);
                data.rlim_cur = (rlim_t) atoi(optarg);
                setrlimit(RLIMIT_CORE, &data);
                break;
            case 'd':
                getcwd(path, sizeof(path));
                printf("%s\n", path);
                break;
            case 'v':
                for (char **env = environ; *env != 0; ++env) {
                    printf("%s\n", *env);
                }
                break;
            case 'V':
                putenv(optarg);
                break;
            default:
                break;
        }
    }

    return 0;
}
