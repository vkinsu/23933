#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

/*struct rlimit {
	rlim_t rlim_cur;    //мягкое ограничение
	rlim_t rlim_max;    //жесткое ограничение
					   //(потолок для rlim_cur)
};*/

int main(int argc, char const* argv[], char* envp[])
{
	char path[1000], p, qw[1000];
	char* name = NULL;
	char* eq = NULL;
	int rez = 0;
	while ((rez = getopt(argc, argv, "iU:spucC:dvV:")) != -1)
	{
		struct rlimit limit;
		switch (rez)
		{
			case 'i':
				printf("Real id user %d.\n", getuid());
				printf("Real id group %d.\n", getgid());
				printf("Effective id user %d.\n", geteuid());
				printf("Effective id group %d.\n", getegid());
				break;
				//case 'U': printf("Qwerty\n"); break;
			case 's':
				if(setpgid(0, 0) == -1)
				{
					perror("setpgid");
				}
				break;
			case 'p':
				printf("Process id %d.\n", getpid());
				printf("Parents process id %d.\n", getppid());
				printf("Group process id %d.\n",getpgrp());
				break;
			case 'u':
				if(getrlimit(RLIMIT_FSIZE, &limit) == 0)
					printf("File size limit %d.\n", limit.rlim_cur);
				else
					perror("getrlimit");
				break;
			case 'U':
				setrlimit(RLIMIT_FSIZE, atoi(optarg));
				break;
			case 'c':
				printf("Max core file %d.\n", RLIMIT_CORE);
				break;
			case 'C':
				//getrlimit(RLIMIT_CORE, &limit);
				//limit.rlim_cur = optarg;
				setrlimit(RLIMIT_CORE, atoi(optarg));
				break;
			case 'd':
				p = getwd(path);
				printf("You are in directory: %s\n", path);
				break;
			case 'v':
				for(int i = 0; envp[i] != NULL; i++)
				{
					printf("%s\n", envp[i]);
				}
				break;
			case 'V':
				name = strdup(optarg);
				eq = strchr(name, '=');
				if (eq)
				{
					*eq = '\0';
					setenv(name, eq, 1);
				}
				free(name);
				break;
			default:
				perror("Invalid option\n");
				break;
		}
    }
}
