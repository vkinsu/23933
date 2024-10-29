#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("Real user ID: %d\n", getuid());
    printf("Effective user ID: %d\n", geteuid());

    FILE *file = fopen("text.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
    }else{
        printf("File opened and closed successfully.\n");
        fclose(file);
    }

    setuid(geteuid());
    printf("After setuid:\n");
    printf("Real user ID: %d\n", getuid());
    printf("Effective user ID: %d\n", geteuid());

    file = fopen("text.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
    }else{
        printf("File opened and closed successfully.\n");
        fclose(file);
    }
    

    return 0;
}
