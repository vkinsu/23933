#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispucU:C:dvV:")) != -1) {
    	struct rlimit lim;
    	
    	char cwd[PATH_MAX];
    	
    	extern char **environ;
    	char **env = environ;
    	
    	char *name = NULL;
    	char *value = NULL;
        switch (opt) {
            case 'i':
		printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
		printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
                break;
            case 's':
                if (setpgid(0, 0) == -1) {
                    perror("setpgid");
                }
                break;
            case 'p':
    		printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgid(0));
                break;
            case 'u':
		if (getrlimit(RLIMIT_FSIZE, &lim) == 0) printf("File size limit: %ld\n", lim.rlim_cur);
		else perror("getrlimit");
                break;
            case 'U':
		lim.rlim_cur = atol(optarg);
		lim.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_FSIZE, &lim) == -1) perror("setrlimit");
                break;
            case 'c':
		if (getrlimit(RLIMIT_CORE, &lim) == 0) printf("Core file size limit: %ld\n", lim.rlim_cur);
		else perror("getrlimit");
                break;
            case 'C':
		lim.rlim_cur = atol(optarg);
		lim.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_CORE, &lim) == -1) perror("setrlimit");
                break;
            case 'd':
		if (getcwd(cwd, sizeof(cwd)) != NULL) printf("Current working directory: %s\n", cwd);
		else perror("getcwd");
                break;
            case 'v':
		while (*env) {
		    printf("%s\n", *env);
		    env++;
		}
                break;
            case 'V':
		name = strtok(optarg, "=");
		value = strtok(NULL, "=");
		if (name && value)
		    if (setenv(name, value, 1) == -1) perror("setenv");
		else fprintf(stderr, "Invalid environment variable format\n");
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

