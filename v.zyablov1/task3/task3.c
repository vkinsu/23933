#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(){
    printf("uid: %d euid: %d\n", getuid(), geteuid());
    FILE *file = fopen("file.txt", "r");
    if(file == NULL) perror("Can't open the file1\n");
    fclose(file);
    setuid(geteuid());

    printf("uid: %d euid: %d\n", getuid(), geteuid());
    file = fopen("file.txt", "r");
    if(file == NULL) perror("Can't open the file2\n");
    fclose(file);
    setuid(getuid());

    printf("uid: %d euid: %d\n", getuid(), geteuid());
    file = fopen("file.txt", "r");
    if(file == NULL) perror("Can't open the file3\n");
    fclose(file);
    setuid(getgid());

    printf("uid: %d euid: %d\n", getuid(), geteuid());
    file = fopen("file.txt", "r");
    if(file == NULL) perror("Can't open the file4\n");
    fclose(file);
    return 0;
}
