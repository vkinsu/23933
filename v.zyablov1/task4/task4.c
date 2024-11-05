#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLEN 100000
//#include <unistd.h>
//#include <sys/resource.h>

typedef struct ListElem{
    struct ListElem *next;
    char *str;
} ListElem;

typedef struct List{
    ListElem *first;
    ListElem *last;
    int len;
} List;

void addStringToList(char *str, List *list){
    ListElem *new = (ListElem*) calloc(1, sizeof(ListElem));
    new->str = (char*) calloc(strlen(str), sizeof(char));
    strcpy(new->str, str);
    if(list->len == 0){
        list->first = new;
        list->last = new;
        list->len += 1;
    } else{
        list->last->next = new;
        list->last = new;
    }
}

int main() {
//    printf("real uid:%d effective? uid:%d", getuid(), geteuid());
    List *list = (List*) calloc(1, sizeof(List));
    char *str = calloc(MAXLEN, sizeof(char));
    printf("Enter something ('.' to interrupt):\n");
    while(fgets(str, MAXLEN,stdin) != NULL){
        if(str[0] == '.') break;
        addStringToList(str, list);
    }
    ListElem *tmp = list->first;
    while(tmp != NULL){
        printf("%s", tmp->str);
        tmp = tmp->next;
    }
    return 0;
}
