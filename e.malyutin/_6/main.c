#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Line {
    size_t offset;
    size_t len;
} Line;

void bye(int signum) {
    printf("\nbye\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [file]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("error while opening file");
        return 1;
    }

    size_t len = 1;
    size_t cap = 2;

    Line *lines = malloc(cap * sizeof(Line));

    char c;
    size_t offset = 0;

    while ((c = getc(file)) != EOF) {
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
        size_t line;
        printf("line? ");

        alarm(5);
        scanf("%lu", &line);
        alarm(0);

        if (line == 0) {
            printf("bye\n");
            break;
        }

        if (line > len) {
            printf("only %lu lines\n", len);
            continue;
        }

        lseek(file->_fileno, lines[line - 1].offset, SEEK_SET);
        char *str = malloc(lines[line - 1].len);
        fread(str, lines[line - 1].len, 1, file);
        fwrite(str, lines[line - 1].len, 1, stdout);
        printf("\n");

        free(str);
    }

    free(lines);
}