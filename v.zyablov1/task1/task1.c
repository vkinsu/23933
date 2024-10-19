#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ulimit.h>
#include <sys/resource.h>

extern char **environ;

void main(int argc, char *argv[]) {
    struct rlimit limits;
    char options[] = "cuvfspidC:V:U:"; /* valid options */
    int c, invalid = 0, dflg = 0, fflg = 0, gflg = 0;
    char *f_ptr, *g_ptr;

    printf("argc equals %d\n", argc);
    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            case 'U':{
                if (ulimit(UL_SETFSIZE,atol(optarg)) == -1)
                    printf("Error ulimit\n");
                break;
            }
            case 'i':
                printf("---i---\nuser id is %d\n", getuid());
                printf("effective user id is %d\n", geteuid());
                break;
            case 's':
                setpgid(getpid(), getppid());
                printf("---s---\nsetpgid %d %d", getpid(), getppid());
                break;
            case 'p':
                printf("---p---\nprocess id is %d\n", getpid());
                printf("parents's process id is %d\n", getppid());
                printf("group's process id is %d\n", getpgrp());
            break;
            case 'd':
                printf("---d---\n");
                char *path = (char *) calloc(1024, 1);
                getcwd(path, 1024);
                printf("current directory is %s\n", path);
                break;
            case 'f':
                fflg++;
                f_ptr = optarg;
                break;
            case 'v':
                printf("---v---\n");
                char **env = environ;
                while (*env != NULL) {
                    printf("%s\n---------------------------\n", *env);
                    env++;
                }
                break;
            case 'u':
                printf("---u---\n");
                getrlimit(RLIMIT_NOFILE, &limits);
                printf("Soft limit for number of open files: %ld\n", limits.rlim_cur);
                printf("Hard limit for number of open files: %ld\n", limits.rlim_max);
                getrlimit(RLIMIT_CPU, &limits);
                printf("Soft limit for cpu time: %ld\n", limits.rlim_cur);
                printf("Hard limit for cpu time: %ld\n", limits.rlim_max);
                getrlimit(RLIMIT_FSIZE, &limits);
                printf("Soft limit for max file size: %ld\n", limits.rlim_cur);
                printf("Hard limit for max file size: %ld\n", limits.rlim_max);
                getrlimit(RLIMIT_DATA, &limits);
                printf("Soft limit for max dynamic mem size: %ld\n", limits.rlim_cur);
                printf("Hard limit for max dynamic mem size: %ld\n", limits.rlim_max);
                getrlimit(RLIMIT_STACK, &limits);
                printf("Soft limit for stack size: %ld\n", limits.rlim_cur);
                printf("Hard limit for stack size: %ld\n", limits.rlim_max);
                getrlimit(RLIMIT_CORE, &limits);
                printf("Soft limit for core file size: %ld\n", limits.rlim_cur);
                printf("Hard limit for core file size: %ld\n", limits.rlim_max);
                break;
            case 'c':
                printf("---c---\n");
                getrlimit(RLIMIT_CORE, &limits);
                printf("Soft limit for core file size: %ld\n", limits.rlim_cur);
                printf("Hard limit for core file size: %ld\n", limits.rlim_max);
            case 'C':
                printf("---C---\n");
                struct rlimit core_limit;
                core_limit.rlim_cur = (long long) optarg;
                printf("new soft limit is %lld\n", core_limit.rlim_cur);
            case 'V':
                printf("---V---\n");
                if(strlen(optarg) == 0){
                    printf("Incorrect -V data, try again\n");
                    break;
                }
                int eqPos;
                strchr(optarg, '=');
                char *name = (char*) calloc(eqPos + 1, sizeof(char));
                memcpy(name, optarg, eqPos + 1);
                char *curVal = getenv(name);
                if(curVal) printf("cur %s val is %s\n", name, curVal);
                else printf("var %s now doesn't exist\n", name);
                char *val = (char*) calloc(strlen(optarg) - eqPos, sizeof(char));
                strcpy(curVal, optarg + eqPos);
                printf(":%s=%s:\n", name, val);
                if(setenv(name, val, 1)) printf("something goes wrong, try again");
                else printf("cur %s val is %s\n", name, curVal); 
//                printf("%d\n", strpbrk(optarg, "="));
            case 'g':
                gflg++;
                g_ptr = optarg;
                break;
            case '?':
                printf("invalid option is %c\n", optopt);
                invalid++;
        }
    }
    printf("dflg equals %d\n", dflg);
    if (fflg)
        printf("f_ptr points to %s\n", f_ptr);
    if (gflg)
        printf("g_ptr points to %s\n", g_ptr);
    printf("invalid equals %d\n", invalid);
    printf("optind equals %d\n", optind);
    if (optind < argc) { printf("next parameter = %s\n", argv[optind]); }
}