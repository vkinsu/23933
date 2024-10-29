#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void try_open(char* fname){
    FILE *f = fopen("Passworded file.txt", "r");
    if(f == NULL){
        perror("\nCan't open the file\n");
    }
    else fclose(f);
}

void print_uid(){
    printf("euid: %u\tuid: %u\n", geteuid(), getuid());
}

int main(){
    print_uid();
    try_open("test.txt");

    setuid(geteuid());

    print_uid();
    try_open("test.txt");
}