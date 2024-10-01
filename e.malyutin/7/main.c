#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct Line {
    size_t offset;
    size_t len;
} Line;

char *file;
Line *lines;
int fd;
size_t size;

void bye(int signum) {
    printf("\nbye\n");

    munmap(file, size);
    close(fd);
    free(lines);

    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [file]\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("error while opening file");
        return 1;
    }

    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    file = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    size_t len = 1;
    size_t cap = 2;

    lines = malloc(cap * sizeof(Line));

    char c;
    size_t offset = 0;

    for (size_t i = 0; i < size; i++) {
        char c = file[i];
        if (c == '\n') {
            if (len + 1 > cap) {
                cap = cap + cap / 2;
                lines = realloc(lines, cap * sizeof(Line));
            }

            lines[len].offset = offset + 1;
            lines[len].len = 0;
            len++;
            offset++;
        } else {
            lines[len - 1].len++;
            offset++;
        }
    }

    signal(SIGALRM, bye);

    while (1) {
        size_t line_n;
        printf("line? ");

        alarm(5);
        scanf("%lu", &line_n);
        alarm(0);

        if (line_n == 0) {
            printf("bye\n");
            break;
        }

        if (line_n > len) {
            printf("only %lu lines\n", len);
            continue;
        }

        Line line = lines[line_n - 1];

        write(0, file + line.offset, line.len);

        printf("\n");
    }

    munmap(file, size);
    close(fd);
    free(lines);
}
