#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int count = 0;

void sigcatch(int sig){
	if (sig == SIGQUIT){
		printf("Signal was %d times\n", count);
		exit(0);
	}
	if (sig == SIGINT){
		printf("\a");
		count ++;
	}
	printf("%d primes computed\n", count);

}

int main(){
	sigset(SIGINT, sigcatch);
	sigset(SIGQUIT, sigcatch);

	while(1)
		pause();

	exit(1);

}
