#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Line {
    size_t offset;
    size_t len;
} Line;

int fd;
Line *lines;

void bye(int signum) {
    printf("\nbye\n");

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

    size_t len = 1;
    size_t cap = 2;

    lines = malloc(cap * sizeof(Line));

    char c;
    size_t offset = 0;

    char buf[2048];
    size_t bytes_read;
    while ((bytes_read = read(fd, buf, sizeof(buf))) != 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            char c = buf[i];
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
        lseek(fd, line.offset, 0);

        char buf[2048];
        size_t chunks = line.len / sizeof(buf);
        size_t rem = line.len % sizeof(buf);

        for (size_t i = 0; i < chunks; i++) {
            read(fd, buf, sizeof(buf));
            write(0, buf, sizeof(buf));
        }

        if (rem > 0) {
            read(fd, buf, rem);
            write(0, buf, rem);
        }

        printf("\n");
    }

    close(fd);
    free(lines);
}