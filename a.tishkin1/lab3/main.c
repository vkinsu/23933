#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    FILE *file;

    if (argc < 2){
        perror("filename argument is missing\n"); return -1;
    }

    printf("Real user id: %u.\nEffective user id: %u.\n", getuid(), geteuid());

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror(argv[1]);
    } else {
        fclose(file);
    }

    setuid(geteuid());
    printf("Real user id: %u.\nEffective user id: %u.\n", getuid(), geteuid());

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror(argv[1]); return -1;
    } else {
        fclose(file);
    }

    return 0;
}
