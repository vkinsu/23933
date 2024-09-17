#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
extern char *tzname[];

int main() {
    printf("real uid: %d\n", getuid());
    printf("effective uid: %d\n", geteuid());

    FILE *f = fopen("file", "r");
    if (f == NULL) {
        perror("can't open file");
    } else {
        fclose(f);
    }

    setuid(geteuid());

    printf("real uid: %d\n", getuid());
    printf("effective uid: %d\n", geteuid());

    f = fopen("file", "r");
    if (f == NULL) {
        perror("can't open file");
    } else {
        fclose(f);
    }
}