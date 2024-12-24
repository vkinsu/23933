#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

/*
Напишите программу, которая запускает команду, заданную в качестве первого аргумента, в виде порожденного процесса. Все остальные аргументы программы передаются этой команде. Затем программа должна дождаться завершения порожденного процесса и распечатать его код завершения.
 
*/

int main(int argc, char *argv[]){
    
    int pid = fork();
    
	if (pid < 0){
		perror("\nfork(2) failed\n");
	}

    if (pid == 0){ // child
		execvp("./child", argv );
		perror("this line should never get printed\n");
		exit(5);
	}
    else if (pid > 0){ // parent
		
		int status;
		pid_t child_pid = waitpid(pid, &status, 0);

		if (child_pid == -1){
			perror("waitpid(2) failed");
			exit(2);
		}

		int exit_code = WEXITSTATUS(status);
		printf("exit status of child process is %d\n", exit_code);
    }
	
	return 0;
}
