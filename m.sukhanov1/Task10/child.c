#include <stdio.h>
#include <string.h>
extern char ** environ;

int main(int argc, char* argv[]){

	for (int i = 0; i < argc; i++){
		printf("%d arg is %s\n", i, argv[i]);
	}
	char **p;
	int i = 1;
	for (p = environ; *p != 0; p++){
		printf("%d ENVVAR is %s\n", i++, *p);
		fflush(stdout);
	}
	printf("amount of argc: %d", argc);
	if (argc > 0){	
		printf("string: %s _____---_____this is strcmp %d\n", argv[1], strcmp(argv[1], "child"));
		if (strcmp(argv[1], "child\0")){
			return 1;
		}
		else{
			return 2;
		}
	}
	else{
		perror("use valid arguments!! ");
	}

	return 0;

}

