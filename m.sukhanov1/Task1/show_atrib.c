#include <stdio.h>
#include <ulimit.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/resource.h>
#define no_argument 		0
#define required_argument 	1
#define optional_argument	2


extern char **environ;

int main(int argc, char *argv[]){

	const char *short_options = "ispuU:cC:dvV:";
	
	
	const struct option long_options[] = {
		{"Unew_ulimit", required_argument, NULL, 'U'},
		{"Csize", required_argument, NULL, 'C' },
		{"Vname=value", required_argument, NULL, 'V'},
		{NULL, 0, NULL, 0}
	};
	
	int rez;
	int option_index;

	while((rez = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1){
		switch(rez){
			case 'i': {
				printf("Effective id: %d\n", geteuid() );
				printf("Effective group id: %d\n", getegid());
				printf("Real id: %d\n", getuid());
				printf("Real group id: %d\n", getgid());
				break;
					  }
			case 's': {
				setpgid(1,1);
				break;
					  }
			case 'p': {
				printf("Id of process: %d\n", getpid());
				printf("Id of precestor process: %d\n", getppid());
				printf("Id of process group: %d\n", getpgrp());	
				break;
					  }
			case 'u': {
				printf("Maximum size of file: %ld\n", ulimit(UL_GETFSIZE));
				break;
					  }
			case 'U': {
				printf("Maximum size of file: %ld\n", ulimit(UL_GETFSIZE));
				long long new_limit = atol(optarg);
				printf("Output of ulimit: %d\n", ulimit(UL_SETFSIZE, new_limit));
				
				if(ulimit(UL_SETFSIZE, new_limit) < 0){
					printf("Unavailable value:    %lld\n", new_limit);
					exit(1);
				}
				printf("New maximum file size: %ld\n", ulimit(UL_GETFSIZE));
				break;
					  }
			case 'c': {
				printf("Maximum size of core-file: %d\n", RLIMIT_CORE);
				break;
					  }
			case 'C': {
				printf("Current maximum size of core-file: %d\n", RLIMIT_CORE);
				struct rlimit rlp;
				rlp.rlim_cur = atoi(optarg);
				if (setrlimit(RLIMIT_CORE, &rlp) < 0){
				    printf("Unavailable value:     %ld", rlp.rlim_cur);
				    exit(2);
				}
				printf("New maximum size of core-file: %ld\n", rlp.rlim_cur);
				
				break;
			}
			case 'd': {
				char *buff = malloc(sizeof(char) * 100);
				printf("Current working directory: %s\n", getcwd(buff, 100));
				//printf("buff: %s", buff);
				free(buff);
				break;
					  }
			case 'v': {
				printf("All environment variables:\n");
				char **p;
				for (p = environ; *p; p++){
					printf("%s\n", *p);
				}
				break;
					}
			case 'V': {

				printf("Variable: %s\n", optarg);
				if (putenv(optarg)){
					printf("There are some troubles");
					exit(2);
				}
				printf("New env: %s\n", getenv(optarg));

				break;
					  }
			case '?': {
				printf("Impossible argument: %s\n", optarg);
					break;
					  }

		}

	}
	return 0;

}













	

