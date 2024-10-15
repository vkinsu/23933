#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, char *argv[], char *envp[]){
	void printenv(const char **);
	printenv(envp);
	putenv("TZ=PST8PDT");
    printenv(envp);
    putenv("WARNING=Don't use envp after putenv()");
    printenv(envp);
    printf("value of WARNING is: %s\n", getenv("WARNING"));



}

void printenv(const char **envp){
    
    char **p;
    
    printf("envp contains %8x\n", envp);
    
    printf("environ contains %8x\n", environ);
    
    printf("My environment variables are:\n");
    

    /* loop stops on encountering a pointer to a NULL address*/
    
    for (p = environ; *p; p++)
        printf ("(%8x) = %8x -> %s\n", p, *p, *p);
        putchar('\n');
}

