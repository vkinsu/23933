#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>

int execvpe(char *file, char *const argv[], char **envp){
    extern char **environ;
    if(envp != NULL) environ = envp;
    printf("Do you want to see new environ? y/n");
    char answ;
    scanf("%c", &answ);
    int counter = 0;
    if(answ == 'y'){
        char **envcpy = environ;
        while(*envcpy != NULL && counter < 10)
            printf("%d %s\n",envcpy, *envcpy);
            envcpy += 1;
            counter += 1;
            if(counter > 1) exit(5);
    }
    printf("execvpe was created");
    return execvp(file, argv);
}


int main(int argc, char *argv[]) {\
//    extern char **environ;
//    printf("\n%d %s\n", argc, argv[0]);
    if(argc < 2){
        perror("You must at least enter program name to exec\n");
        exit(1);
    }
    printf("Enter number of environment params you want to set:\n");
    int n = 0;
    scanf("%d", &n);
    if(n < 0){
        perror("Incorrect number of params");
        exit(1);
    }
    char **new_env = calloc(n + 1, sizeof(char*));
    if(n > 0){
        printf("Enter environment params you want to set new process (param=val) separates by '\\n'):");
        for(int i = 0; i < n; i++){
            size_t len = 100;
            new_env[i] = (char*) calloc(len + 1, sizeof(char));
            int read = getline(&new_env[i], &len, stdin);
            if(read < 3){
                perror("You entered incorrect param=val");
                i -= 1;
                continue;
            }
            new_env[i][len] = '\000';
        }
    }
//    char *const new_env[] = {"USER=ElevenLabUser", NULL};
    new_env[n] = NULL;
    execvpe(argv[1], argv + 1, new_env);
    perror("Something went wrong\n");
    return 1;
}