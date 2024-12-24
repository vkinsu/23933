#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern pid_t waitpid(pid_t pid, int *status, int options);

int main(){
	pid_t pid = fork();	
	
	if (pid < 0){
		perror("fork failed");
		exit(1);
	}

	if (pid == 0){
		execlp("cat", "cat", "longfile.txt", NULL);
		perror("execlp failed");
		exit(2);
	}
	
	if (pid > 0){
		int status;
		waitpid(pid, &status, 0);
		printf("Parent: text from parent process after completing of precestor's");

	}
	return 0;
}
