#include <stdio.h>
#include <stdlib.h>
#include "list.h"

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