#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

void print_uids() {
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
}

int main() {
    FILE *file;
    const char *filename = "datafile.txt";

    print_uids();

    file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File opened successfully\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1) {
        perror("Error setting UID");
        exit(EXIT_FAILURE);
    }

    print_uids();

    file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File opened successfully\n");
        fclose(file);
    }

    return 0;
}
