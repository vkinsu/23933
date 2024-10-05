#include "list.h"

void init(list* ls){
    ls->beg = (list_elem*)malloc(sizeof(list_elem));
    ls->beg->string = NULL;
    ls->beg->next = NULL;
    ls->end = ls->beg;
}

void push(list* ls, char* str, int len){
    ls->end->string = (char*)malloc(sizeof(char) * len);
    strcpy(ls->end->string, str);
    ls->end->next = (list_elem*)malloc(sizeof(list_elem));
    ls->end = ls->end->next;
}

void print_list(list* ls){
    printf("\nList:\n");
    for(list_elem* iter = ls->beg; iter != ls->end; iter = iter->next){
        printf("%s", iter->string);
    }
}

void free_list(list* ls){
    list_elem* iter = ls->beg;
    list_elem* iter_next = iter->next;
    while(iter_next != NULL){
        free(iter);
        iter = iter_next;
        iter_next = iter->next;
    }
    free(iter);
}