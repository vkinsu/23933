#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

void count_empty_lines(const char *filename) {
    // Формируем команду для wc, чтобы подсчитать количество строк в файле
    FILE *pipe = popen("wc --lines --files0-from=-", "w");
    if (!pipe) {
        perror("popen failed");
        exit(1);
    }
    fprintf(pipe, "%s", filename);
    pclose(pipe);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    count_empty_lines(argv[1]);
    return 0;
}

