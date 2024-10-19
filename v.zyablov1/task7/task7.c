#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define BUF_LEN 10000

typedef struct File{
    unsigned int len;
    unsigned int arrLen;
    char **lines;
} File;

void timeoutAlarm(){
    fprintf(stderr, "program was inactive for 5 seconds and it has been killed\n");
    exit(0);
}

void addToArr(File *file, char* str){
    if(file->len >= file->arrLen){
        file->arrLen += 20;
        file->lines = (char**) realloc(file->lines, sizeof(char*) * file->arrLen);
    }
    file->lines[file->len] = str;
    file->len += 1;
}


int main(int argc, char *argv[]) {
    char inp[50];
    printf("Enter input file name below (defauld=file.txt):");
    
    signal(SIGALRM, timeoutAlarm);
    alarm(5);
    
    scanf("%49s", inp);
    // Проверяем, было ли введено что-то пользователем
    if (inp[0] == '\0') {
        strcpy(inp, "file.txt");  // Присваиваем значение по умолчанию
    }
    File file;
    file.lines = 0;
    file.len = 0;
    file.arrLen = 20;
    file.lines = (char**) calloc(file.arrLen, sizeof(char*));
    FILE *input = fopen(inp, "r");
    if(!input){
        fprintf(stderr, "fsync failed\n");
        exit(1);
    }


    char *buf = (char*) calloc(BUF_LEN, sizeof(char));
    while(fgets(buf, BUF_LEN, input)){
        addToArr(&file, buf);
        buf = (char*) calloc(BUF_LEN, sizeof(char));
    }
    int strNum = 1;
    printf("\nEnter number of string which you want to see\n");
    scanf("%d", &strNum);
    while(strNum != 0){
        signal(SIGALRM, timeoutAlarm);
        alarm(5);
        if(strNum <= 0 || strNum > file.len){
            fprintf(stderr, "String index out of range");
            continue;
        }
        printf("%d'th string is:%s", strNum, file.lines[strNum - 1]);
        printf("\nEnter number of string which you want to see\n");
        scanf("%d", &strNum);
    }
    for(int i = 0; i < file.len; i++) free(file.lines[i]);
    free(file.lines);
    return 0;
}
