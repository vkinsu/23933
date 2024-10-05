#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void try_open(char* fname){
    FILE *f = fopen("test.txt", "r");
    if(f == NULL){
        perror("\nCan't open the file\n");
    }
    else fclose(f);
}

int main(){
    printf("euid: %u\tuid: %u\n", geteuid(), getuid());
    try_open("test.txt");

    setuid(geteuid());

    printf("euid: %u\tuid: %u\n", geteuid(), getuid());
    try_open("test.txt");
}