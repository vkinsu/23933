#include "parse.h"

int parse(){
    int size = 3;
    char buff[3];

    if(fgets(buff, size, stdin) == NULL){
        return -1;
    }

    if(buff[0] == '0'){
        exit(0);
    }

    int num = 0;
    while(1){
        for(int i = 0; i < size; i++){
            if(('0' <= buff[i]) && (buff[i] <= '9')){
                num *= 10;
                num += buff[i] - 48;
            }
            if(buff[i] == '\0' || buff[i] == '\n'){
                return num == 0 ? -1 : num;
            }
            else{
                return -1;
            }
        }

        if(fgets(buff, size, stdin) == NULL){
            return -1;
        }
    }
}