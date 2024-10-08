#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

extern char **environ;

void userIDs(){
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();
    printf("Real uID: %d\nEffective uID: %d\nReal uID: %d\nEffective uID: %d\n", real_uid, effective_uid, real_gid, effective_gid);
}

void processLeader(){
    setpgid(0, 0);
}

void processIDs(){
    pid_t ProcessID = getpid();
    pid_t ParentProcessID = getppid();
    pid_t ProcessGroupID = getpgid(ProcessID);
    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", ProcessID, ParentProcessID, ProcessGroupID);
}

void print_ulimit(){
    printf("Ulimit: %ld\n", ulimit(1, 0));
}

void set_ulimit(const char *opt){
    char *endptr;
    unsigned long val = strtoul(opt, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid number: %s\n", opt);
        return 1;
    }

    if (ulimit(2, val) == -1) {
        fprintf(stderr, "Failed to set new ulimit.\n");
    }
}

void coreFileSize(){
    struct rlimit rl;
    getrlimit(RLIMIT_CORE, &rl);
    printf("Core File Size: %ld\n", rl.rlim_cur);
}

void setCoreFileSize(const char *opt){
    struct rlimit rl;
    getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = atol(opt);
    if(setrlimit(RLIMIT_CORE, &rl) == -1) fprintf(stderr, "failed setrlimit\n");
}

void currentDir(){
    printf("Current working directory: %s\n", getcwd(NULL, 1000));
}

void envVar(){
    printf("Environment variables:\n");
    for (char **e = environ; *e; e++) printf("%s\n", *e); 
}

void setEnvVar(const char *opt){
    putenv(opt);
}
int main(int argc, char **argv){

    int option;
    if (argc == 1) return 1;

    while ((option = getopt(argc, argv, "ispuU:cC:dvV:h")) != -1)
    {
        switch (option) {
            case 'i': userIDs(); break;
            case 's': processLeader(); break;
            case 'p': processIDs(); break;
            case 'u': print_ulimit(); break;
            case 'U': set_ulimit(optarg); break;
            case 'c': coreFileSize(); break;
            case 'C': setCoreFileSize(optarg); break;
            case 'd': currentDir(); break;
            case 'v': envVar(); break;
            case 'V': setEnvVar(optarg); break;
            default: {
                fprintf(stderr, "Invalid option\n");
                return 1;
            }
        }
    }

    return 0;
}
