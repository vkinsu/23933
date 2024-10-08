#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/resource.h>
#include <string.h>

extern char** environ;

int main(int argc, char *argv[]){
	char oper;

	while((oper = getopt(argc, argv, "ispuU:cC:dvV:")) != -1){
		int a;
		long new_limit;
		struct rlimit rlp;
		char** ptr = environ;
		char* var,* eq, *pathname;
		switch (oper){

			case 'i':
				printf ("UID: %d \nEUID: %d \nGID: %d \nEGID: %d \n",
				getuid(),
				geteuid(),
				getgid(),
				getegid());
			break;

			case 's':
				a = setpgid(0, 0);
				if (a == 0) printf("Sucsrsfull\n");
				else printf("Unsucsrsfull\n");
			break;

			case 'p':
				printf ("PID: %d \nPPID: %d\nPGID: %d\n",
				getpid(),
				getppid(),
				getpgrp());
			break;

			case 'u':
				if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
						perror("Failed to get ulimit\n");
				} else {
						printf("Ulimit value: %lu\n", rlp.rlim_max);
				}
				break;

			case 'U':
				new_limit = strtol(optarg, NULL, 10);

				if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
						perror("Failed to get ulimit\n");
				}
				rlp.rlim_cur = new_limit;
				if (-1 == setrlimit(RLIMIT_FSIZE, &rlp)) {
						perror("Failed to resize ulimit\n");
				} else {
						printf("Sucsrsfull! New ulimit value: %lu \n", rlp.rlim_cur);
					}
			break;

			case 'c':
				if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
						perror("Failed to get ulimit\n");
				} else {
						printf("Ulimit value: %lu\n", rlp.rlim_max);
				}
			break;
			case 'C':
				new_limit = strtol(optarg, NULL, 10);

				if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
						perror("Failed to get ulimit\n");
				}
				rlp.rlim_cur = new_limit;
				if (-1 == setrlimit(RLIMIT_CORE, &rlp)) {
						perror("Failed to resize ulimit\n");
				} else {
						printf("Sucsrsfull! New ulimit value: %lu \n", rlp.rlim_cur);
					}
			break;
			case 'd':
				pathname = getenv("PWD");

				if (NULL == pathname) {
						perror("Failed to get the current directory\n");
				} else {
						printf("Current directory: %s\n", pathname);
				}
			break;
			case 'v':
				for ( ; *ptr != NULL; ptr++){
						printf("%s\n", *ptr);
				}
			break;
			case 'V':
				var = strdup(optarg);
				eq = strchr(var, '=');
				if (eq){
					*eq = '\0';
					if (-1 == setenv(var, eq + 1, 1)){
						perror("Failed!\n");
					}
				}
			break;
		}
	}
}
