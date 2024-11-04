#include "table.h"

void print_table(table *t, int flen){
    printf("\nnumber indent_len str_len\n");
    for(int i = 0; i < t->lines; i++){
        printf("%d\t%d\t%d\n", i + 1, 
                t->matrix[i].indent_len,
                t->matrix[i].str_len);
    }
    printf("lines: %d, flen: %d\n\n", t->lines, flen);
}

void new_line(table *t){
    t->lines += 1;
    t->matrix = (str_info*)realloc(t->matrix, sizeof(str_info) * t->lines);
    t->matrix[t->lines - 1].indent_len = 0;
    t->matrix[t->lines - 1].str_len = 0;
}