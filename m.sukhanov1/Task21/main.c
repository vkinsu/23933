#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int count = 0;

void sigcatch(int sig){
	if (sig == SIGQUIT){
		printf("Signal was %d times\n", count);
		exit(0);
	}
	if (sig == SIGINT){
		write(fileno(stdout), "\a", 1);
		count ++;
	}
	

}

int main(){
	signal(SIGINT, sigcatch);
	signal(SIGQUIT, sigcatch);

	while(1)
		pause();
	exit(1);

}
