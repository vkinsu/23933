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

int main(){
    list strings;
    init(&strings);
    char str[1000] = {'\0'};
    while(str[0] != '.'){
        fgets(str, 1000, stdin);
        push(&strings, str, strlen(str) + 1);
    }
    print_list(&strings);
    free_list(&strings);
}