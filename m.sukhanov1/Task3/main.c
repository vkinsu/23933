#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main(){
	printf("Real user id: %d\n", getuid());
	printf("Effective user id: %d\n", geteuid());
	FILE* in = fopen("text", "r");
	if (in == NULL){
		perror("Could not open this file!");
		exit(1);
	}


	fclose(in);
	setuid(geteuid());
	printf("Real user id: %d\n", getuid());
	printf("Effective user id: %d\n", geteuid());

	exit(0);
}
