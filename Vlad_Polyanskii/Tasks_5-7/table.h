#ifndef TABLE
#define TABLE

#include <stdio.h>
#include <stdlib.h>

typedef struct str_info{
    int str_len;
    int indent_len;
} str_info;

typedef struct table{
    int lines;
    str_info* matrix;
} table;

void print_table(table *t, int  flen);
void new_line(table *t);

#endif