#ifndef LIST
#define LIST


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list_elem{
    char *string;
    struct list_elem *next;
} list_elem;

typedef struct list{
    list_elem* beg;
    list_elem* end;
} list;

void init(list* ls);
void push(list* ls, char* str, int len);
void print_list(list* ls);
void free_list(list* ls);


#endif