#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [file]\n", argv[0]);
        return 1;
    }

    char editor[] = "nvim";

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("can't open file");
        return 1;
    }

    flock(fd, LOCK_EX);

    size_t flen = strlen(argv[1]);
    char *command = malloc(sizeof(editor) + flen + 1);
    snprintf(command, sizeof(editor) + flen + 1, "%s %s", editor, argv[1]);

    system(command);

    flock(fd, LOCK_UN);
}