#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include<stdlib.h>
#include <stdio.h>
#include <string.h>

extern char *optarg;
extern int optind, opterr, optopt;
extern char **environ;

int main(int argc, char** argv){
    char* opts = "ispuU:cC:dvV:";
    char opt;
    while ((opt = getopt(argc, argv, opts)) != -1){
        switch (opt){
            case 'i':{
                printf("euid: %u\tuid: %u\n", geteuid(), getuid());
                printf("egid: %u\tgid: %u\n", getegid(), getgid());
                break;
            }
            case 's':{
                printf("%d", setpgid(0, getpgid(0)));
                break;
            }
            case 'p':{
                printf("pid: %u\tppid: %u\tpgid: %u\n", getpid(), getppid(), getpgid(0));
                break;
            }
            case 'u':{
                struct rlimit fsize;
                getrlimit(RLIMIT_FSIZE, &fsize);
                printf("FSIZE_CUR: %lu\t FSIZE_MAX: %lu\n", fsize.rlim_cur, fsize.rlim_max);
                break;
            }
            case 'U':{
                int new_rlim = atol(optarg);
                if(new_rlim < 0){
                    printf("Error: needs rlim_t aka unsigned int\n");
                    exit(1);
                }
                struct rlimit fsize;
                fsize.rlim_cur = (unsigned)new_rlim;
                fsize.rlim_max = (unsigned)new_rlim;
                printf("%d\n", setrlimit(RLIMIT_FSIZE, &fsize));
                break;
            }
            case 'c':{
                struct rlimit cfsize;
                getrlimit(RLIMIT_DATA, &cfsize);
                printf("DATA_CUR: %lu\t DATA_MAX: %lu\n", cfsize.rlim_cur, cfsize.rlim_max);
                break;
            }
            case 'C':{
                int new_rlim = atol(optarg);
                if(new_rlim < 0){
                    printf("Error: needs rlim_t aka unsigned int\n");
                    exit(1);
                }
                struct rlimit cfsize;
                cfsize.rlim_cur = (unsigned)new_rlim;
                cfsize.rlim_max = (unsigned)new_rlim;
                printf("%d\n", setrlimit(RLIMIT_DATA, &cfsize));
                break;
            }
            case 'd':{
                printf("%s\n", getenv("PWD"));
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
                char* value = (char*)malloc(sizeof(char));
                value[0] = '\0';

                int i;
                for(i = 0; optarg[i] != '=' && optarg[i] != '\0'; i++);
                name = (char*)malloc(sizeof(char) * (i + 1));
                memcpy(name, optarg, sizeof(char) * i);
                name[i] = '\0';

                if(optarg[i] != '\0'){
                    value = &optarg[++i];
                }

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