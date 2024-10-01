#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

extern char *optarg;

extern int optind, opterr, optopt;

extern char **environ;

int main(int argc, char** argv){
    if(argc == 1){
        printf("There are no arguments.\n");
        return 0;
    }

    char* opts = "ispuU:cC:dvV:";
    int opt;
    while((opt = getopt(argc, argv, opts)) != -1){
        switch (opt){
            case 'i':{
                printf("euid: %u\tuid: %u\n", geteuid(), getuid());
                printf("egid: %u\tgid: %u\n", getegid(), getgid());
                break;
            }
            case 's':{
                printf("%d\n", setpgid(0, getpgid(0)));
                break;
            }
            case 'p':{
                printf("pid: %u\tppid: %u\tpgid: %u\n", getpid(), getppid(), getpgid(0));
                break;
            }
            case 'u':{
                struct rlimit flimit;
                if(!getrlimit(RLIMIT_FSIZE, &flimit))
                    printf("ulimit_cur: %lu\tulimit_max: %lu\n", flimit.rlim_cur, flimit.rlim_max);
                else printf("-1\n");
                break;
            }
            case 'U':{
                int new_value = strtol(optarg, NULL, 10);
                if(new_value < 0){
                    printf("Error: Needs type rlim_t aka unsigned long\n");
                    return -1;
                }
                struct rlimit flimit;
                flimit.rlim_cur = (rlim_t)new_value;
                flimit.rlim_max = (rlim_t)new_value;
                printf("%d\n", setrlimit(RLIMIT_FSIZE, &flimit));
                /*printf("%d\n", getrlimit(RLIMIT_FSIZE, &ulimit));
                printf("ulimit_cur: %lu\tulimit_max: %lu\n", ulimit.rlim_cur, ulimit.rlim_max);*/
                break;
            }
            case 'c':{
                struct rlimit core_file;
                if(!getrlimit(RLIMIT_DATA, &core_file))
                    printf("ulimit_cur: %lu\tulimit_max: %lu\n", core_file.rlim_cur, core_file.rlim_max);
                else printf("-1\n");
                break;
            }
            case 'C':{
                int new_value = strtol(optarg, NULL, 10);
                if(new_value < 0){
                    printf("Error: Needs type rlim_t aka unsigned long\n");
                    return -1;
                }
                struct rlimit core_file;
                core_file.rlim_cur = (rlim_t)new_value;
                core_file.rlim_max = (rlim_t)new_value;
                printf("%d\n", setrlimit(RLIMIT_DATA, &core_file));
                /*printf("%d\n", getrlimit(RLIMIT_DATA, &core_file));
                printf("core_file_cur: %lu\tcore_file_max: %lu\n", core_file.rlim_cur, core_file.rlim_max);*/
                break;
            }
            case 'd':{
                printf("curr_dir: %s\n", getenv("PWD"));
                break;
            }
            case 'v':{
                for(int i = 0; environ[i] != NULL; i++){
                    printf("%s\n\n", environ[i]);
                }
                break;
            }
            case 'V':{
                char* name = NULL;
                char* value = NULL;

                int i;
                for(i = 0; optarg[i] != '='; i++);
                name = (char*)malloc(sizeof(char) * (i + 1));
                memcpy(name, optarg, sizeof(char) * i);
                name[i++] = '\0';

                int value_size = 0;
                char *from = &optarg[i];
                for(; optarg[i] != '\0'; i++, value_size++);
                value = (char*)malloc(sizeof(char) * ++value_size);
                memcpy(value, from, value_size);
                value[value_size] = '\0';

                printf("%s=%s\n", name, value);
                printf("%d\n", setenv(name, value, 1));
                break;
            }     
            default:{
                break;
            }
        }
    }
}