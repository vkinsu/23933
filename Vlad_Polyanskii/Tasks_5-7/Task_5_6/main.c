#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "../parse.h"
#include "../table.h"

int fd;
int flen = 0;
char* buff = NULL;
table t;

void get_strs_info(table* t, char* text){
    int i = 0;
    while(1){
        while(text[i] == ' '){
            t->matrix[t->lines - 1].indent_len++;
            flen++;
            i++;
        }

        while(text[i] != '\0' && text[i] != '\n'){
            t->matrix[t->lines - 1].str_len++;
            flen++;
            i++;
        }
        t->matrix[t->lines - 1].str_len++;
        flen++;

        if(text[i] == '\0'){
            return;
        }
        else{
            new_line(t);
        }
        i++;
    }
}

void get_table(int fd){
    struct stat f_info;
    fstat(fd, &f_info);

    t.lines = 0, t.matrix = NULL;
    new_line(&t);

    char* text = (char*)malloc(f_info.st_size);
    if(read(fd, text, f_info.st_size) == -1){
        perror("read in get_table has returned -1");
        exit(1);
    }

    get_strs_info(&t, text);
    buff = (char*)malloc(sizeof(char) * flen);
    buff[flen - 1] = '\0';

    free(text);
}

void print_str(int line_num, table* t){
    line_num -= 1;
    off_t offset = 0;
    for(int i = 0; i < line_num; i++){
        offset += t->matrix[i].indent_len;
        offset += t->matrix[i].str_len;
    }
    offset += t->matrix[line_num].indent_len;

    lseek(fd, offset * sizeof(char), SEEK_SET);

    int len = t->matrix[line_num].str_len;

    if(read(fd, buff, sizeof(char) * len) == -1){
        perror("read in print_str has returned -1");
        exit(1);
    }

    buff[len] = '\0';
    printf("%s\n", buff);
}

void free_global_mem(){
    free(buff);
    free(t.matrix);
}

void print_file(int signum){
    lseek(fd, 0, SEEK_SET);

    if(read(fd, buff, sizeof(char) * flen) == -1){
        perror("read in print_file has returned -1");
        exit(1);
    }

    printf("\n\nFILE:\n%s\n", buff);
    free_global_mem();
    exit(0);
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("ERROR\nWrong number of parametres\n");
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("\nERROR\nCannot open the file");
        return 1;
    }

    get_table(fd);
    print_table(&t, flen);
    
    printf("USAGE:\nEnter line numbers one at time\nPrint 0 to end\n\n");
    while(1){
        printf("in: ");
        signal(SIGALRM, print_file);
        alarm(5);
        int line_num = parse();
        if(line_num == -1 || line_num > t.lines){
            continue;
        }
        printf("out: ");
        print_str(line_num, &t);
    }
    
    free_global_mem();
    return 0;
}