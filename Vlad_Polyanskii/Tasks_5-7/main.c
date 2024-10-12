#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

int fd;
int flen = 0;

typedef struct str_info{
    int str_len;
    int indent_len;
} str_info;

typedef struct table{
    int lines;
    str_info* matrix;
} table;

void print_table(table* t){
    printf("\nnumber indent_len str_len\n");
    for(int i = 0; i < t->lines; i++){
        printf("%d\t%d\t%d\n", i + 1, 
                t->matrix[i].indent_len,
                t->matrix[i].str_len);
    }
    printf("lines: %d, flen: %d\n\n", t->lines, flen);
}

void new_line(table* t){
    t->lines += 1;
    t->matrix = (str_info*)realloc(t->matrix, sizeof(str_info) * t->lines);
    t->matrix[t->lines - 1].indent_len = 0;
    t->matrix[t->lines - 1].str_len = 0;
}

int count_indents(table* t){
    char sym;
    while(read(fd, &sym, sizeof(char)) != 0){
        flen += 1;
        if(sym == ' '){
            t->matrix[t->lines - 1].indent_len += 1;
        }
        else{
            t->matrix[t->lines - 1].str_len += 1;
            if(sym == '\0'){
                return 0;
            }
            if(sym == '\n'){
                return 1;
            }
            break;
        }
    }
}

int count_len(table* t){
    char sym;
    while(read(fd, &sym, sizeof(char)) != 0){
        flen += 1;
        t->matrix[t->lines - 1].str_len += 1;
        if(sym == '\n'){
            return 1;
        }
        if(sym == '\0'){
            return 0;
        }
    }
}

table* get_strs_info(int fd){
    table* t = (table*)malloc(sizeof(table));
    t->lines = 0, t->matrix = NULL; 
    new_line(t);
    while (1){
        if(count_indents(t) == 0){
            break;
        }
        if(count_len(t) == 0){
            break;
        }
        new_line(t); 
    }
    return t;
}

void print_str(table* t, int line){
    line -= 1;
    off_t offset = 0;
    for(int i = 0; i < line; i++){
        offset += t->matrix[i].indent_len;
        offset += t->matrix[i].str_len;
    }
    offset += t->matrix[line].indent_len;

    lseek(fd, offset * sizeof(char), SEEK_SET);

    char *buff = (char*)malloc(sizeof(char));
    int len = t->matrix[line].str_len;
    read(fd, buff, sizeof(char) * (len - 1));
    printf("%s\n", buff);
}

void print_file(int signum){
    char *buff = (char*)malloc(sizeof(char) * flen);
    lseek(fd, 0, SEEK_SET);
    read(fd, buff, sizeof(char) * flen);
    printf("\n\nFILE:\n%s\n", buff);
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

    table* strs_info = get_strs_info(fd);
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
        print_str(strs_info, line);
    }
    
    free(strs_info->matrix);
    free(strs_info);
    return 0;
}