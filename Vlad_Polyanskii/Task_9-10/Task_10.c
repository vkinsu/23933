#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vec{
    char* str;
    int size;
    int capacity;
} vec;

void insert(vec* v, char sym){
    if(v->capacity == v->size){
        v->capacity *= 2;
        v->str = (char*)realloc(v->str, sizeof(char) * v->capacity);
    }
    v->str[v->size] = sym;
    v->size++;
}

char* concatCommands(int argc, char** argv){
    vec res;
    res.size = strlen(argv[1]) + 1;
    res.str = (char*)malloc(sizeof(char) * res.size);
    strcpy(res.str, argv[1]);
    res.capacity = res.size;
    res.str[res.size - 1] = ' ';
    for(int i = 2; i < argc; i++){
        for(int j = 0; argv[i][j] != '\0'; j++){
            insert(&res, argv[i][j]);
        }
        insert(&res, ' ');
    }
    return res.str;
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Wrong number of parametres\n");
        return 1;
    }
    char* arg = concatCommands(argc, argv);
    FILE* child_proc = popen(arg, "w");
    if (child_proc == NULL){
        perror("Can't create child process");
        return 1;
    }
    wait(NULL);
    printf("\nParent message\n");
    pclose(child_proc);
    free(arg);
    return 0;
}