#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

void print_user_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
}

void open_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
    } else {
        printf("File opened successfully.\n");
        fclose(file);
    }
}

int main() {
    const char *filename = "task3.txt";

    
    printf("Before setuid:\n");
    print_user_ids();

    
    printf("Trying to open file before setuid:\n");
    open_file(filename);

    
    if (setuid(getuid()) == -1) {
        perror("Failed to set UID");
        exit(EXIT_FAILURE);
    }

    
    printf("\nAfter setuid:\n");
    print_user_ids();
    printf("Trying to open file after setuid:\n");
    open_file(filename);

    return 0;
}

