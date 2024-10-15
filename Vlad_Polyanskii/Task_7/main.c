#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>

typedef struct str_info{
    int str_len;
    int indent_len;
} str_info;

typedef struct table{
    int lines;
    str_info* matrix;
} table;

int fd;
int flen = 0;
table* strs_info = NULL;
char* pa;

void print_table(){
    printf("\nnumber indent_len str_len\n");
    for(int i = 0; i < strs_info->lines; i++){
        printf("%d\t%d\t%d\n", i + 1, 
                strs_info->matrix[i].indent_len,
                strs_info->matrix[i].str_len);
    }
    printf("lines: %d, flen: %d\n\n", strs_info->lines, flen);
}

void new_line(){
    strs_info->lines += 1;
    strs_info->matrix = (str_info*)realloc(strs_info->matrix, sizeof(str_info) * strs_info->lines);
    strs_info->matrix[strs_info->lines - 1].indent_len = 0;
    strs_info->matrix[strs_info->lines - 1].str_len = 0;
}

int count_indents(int* i){
    while(1){
        flen += 1;
        if(pa[*i] == ' '){
            strs_info->matrix[strs_info->lines - 1].indent_len += 1;
            *i += 1;
        }
        else{
            strs_info->matrix[strs_info->lines - 1].str_len += 1;
            if(pa[*i] == '\0'){
                return 0;
            }
            else return 1;
        }
    }
}

int count_len(int* i){
    while(1){
        flen += 1;
        strs_info->matrix[strs_info->lines - 1].str_len += 1;
        if(pa[*i] == '\n'){
            return 1;
        }
        else if(pa[*i] == '\0'){
            return 0;
        }
        *i += 1;
    }
}

void get_strs_info(int fd){
    strs_info = (table*)malloc(sizeof(table));
    strs_info->lines = 0, strs_info->matrix = NULL;
    new_line();
    pa = mmap(NULL, sizeof(char), PROT_READ, MAP_SHARED, fd, 0);
    if(pa == MAP_FAILED){
        perror("MAP_FAILED:");
        printf("\n");
        exit(1);
    }

    int i = 0;
    while (1){
        if(count_indents(&i) == 0){
            break;
        }
        else i++;

        if(count_len(&i) == 0){
            break;
        }
        else i++;
        new_line(); 
    }
}

void print_str(int line){
    line -= 1;
    int beg = 0;
    for(int i = 0; i < line; i++){
        beg += strs_info->matrix[i].indent_len;
        beg += strs_info->matrix[i].str_len;
    }
    beg += strs_info->matrix[line].indent_len;

    int end = strs_info->matrix[line].str_len + beg;
    for(int i = beg; i < end; i++){
        printf("%c", pa[i]);
    }
    printf("\n");
}

void free_mem(){
    free(strs_info->matrix);
    free(strs_info);
    munmap(pa, flen);
}

void print_file(int signum){
    printf("\n\nFILE:\n%s\n", pa);
    free_mem();
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

    get_strs_info(fd);
    print_table(strs_info);
    
    printf("USAGE:\nEnter line numbers one at time\nPrint 0 to end\n\n");
    int line;
    while(1){
        printf("in: ");
        signal(SIGALRM, print_file);
        alarm(5);
        scanf("%d", &line);
        if(line == 0 || line < 0  || line > strs_info->lines){
            return 0;
        }
        printf("out: ");
        print_str(line);
    }
    
    free_mem();
    return 0;
}