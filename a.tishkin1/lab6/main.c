#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


typedef struct row {
    unsigned int pos;
    unsigned int len;
} row;

unsigned int str_count = 1;
FILE *file;
row *table;

void Timeout() {
    printf("\nAll file:\n");

    fseek(file, 0, 0);

    for (int i = 0; i < table[str_count - 1].pos + table[str_count - 1].len; ++i) {
        putc(getc(file), stdout);
    }
    putc('\n', stdout);

    _Exit(0);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        perror("run without arguments");
        return 0;
    }

    file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("can't open file");
        return 0;
    }

    // get count of strings in file
    char ch;

    while ((ch = getc(file)) != EOF) {
        if (ch == '\n') {
            ++str_count;
        }
    }

    table = malloc(sizeof(row) * str_count);

    // fill the table of offsets and lengths
    unsigned int cur_pos = 0;
    unsigned int cur = 0;
    unsigned int cur_str_len = 0;

    fseek(file, 0, 0);
    while ((ch = getc(file)) != EOF) {
        if (ch == '\n') {
            table[cur++].len = cur_str_len;
            table[cur].pos = cur_pos + 1;

            cur_str_len = 0;
        } else {
            ++cur_str_len;
        }

        ++cur_pos;
    }
    table[cur].len = cur_pos - table[cur].pos;

    // set timeout
    signal(SIGALRM, Timeout);

    // receive requests from user
    unsigned int n = 0;

    printf("Enter string number: ");
    alarm(5);
    while (scanf("%d", &n) != 0) {
        if (n == 0) { break; }
        if (n > str_count) {
            perror("string number out of range\n");
            printf("\nEnter string number: ");
            continue;
        }

        fseek(file, table[n - 1].pos, 0);

        for (int i = 0; i < table[n - 1].len; ++i) {
            putc(getc(file), stdout);
        }

        printf("\nEnter string number: ");
        alarm(5);
    }
    putc('\n', stdout);

    fclose(file);
    free(table);
    return 0;
}
