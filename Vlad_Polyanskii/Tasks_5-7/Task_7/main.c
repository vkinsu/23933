#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include "../parse.h"
#include "../table.h"

int fd;
int flen = 0;
char* pa;
table t;

void get_strs_info(table* t){
    int i = 0;
    while(1){
        while(pa[i] == ' '){
            t->matrix[t->lines - 1].indent_len++;
            flen++;
            i++;
        }

        while(pa[i] != '\0' && pa[i] != '\n'){
            t->matrix[t->lines - 1].str_len++;
            flen++;
            i++;
        }
        t->matrix[t->lines - 1].str_len++;
        flen++;

        if(pa[i] == '\0'){
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

    pa = mmap(NULL, f_info.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(pa == MAP_FAILED){
        perror("MAP_FAILED:");
        printf("\n");
        exit(1);
    }
    get_strs_info(&t);
}

void print_str(int line, table *t){
    line -= 1;
    int beg = 0;
    for(int i = 0; i < line; i++){
        beg += t->matrix[i].indent_len;
        beg += t->matrix[i].str_len;
    }
    beg += t->matrix[line].indent_len;

    int end = t->matrix[line].str_len + beg;
    for(int i = beg; i < end; i++){
        printf("%c", pa[i]);
    }
    printf("\n");
}

void free_global_mem(){
    free(t.matrix);
    munmap(pa, flen);
}

void print_file(int signum){
    printf("\n\nFILE:\n%s\n", pa);
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
        printf("%d\n", line_num);
        if(line_num == -1 || line_num > t.lines){
            continue;
        }
        printf("out: ");
        print_str(line_num, &t);
    }
    
    free_global_mem();
    return 0;
}