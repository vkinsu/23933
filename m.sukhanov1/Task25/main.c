#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(){
	int pipe_fd[2]; // array for descriptors (input, output)
	pid_t pid;
	char buffer[256];

	if (pipe(pipe_fd) == -1){
		perror("failed creation of pipe\n");
		exit(1);
	}

	pid = fork();

	if (pid < -1){
		perror("for failed\n");
		exit(2);
	}

	if (pid == 0){ // child
		read(pipe_fd[0], buffer, sizeof(buffer));
		for(int i = 0; i < sizeof(buffer) && buffer[i] != '\0'; i++ ){
			buffer[i] = toupper(buffer[i]);
		}
		write(fileno(stdout), buffer, sizeof(buffer));
	}
	else{ // parent

		const char* message = "Hello from parent process!!\n";
		write(pipe_fd[1], message, strlen(message) + 1);
			
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	return 0;

}
