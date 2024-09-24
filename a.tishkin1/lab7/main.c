#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <fcntl.h>
#include <sys/mman.h>

typedef struct row {
    unsigned int pos;
    unsigned int len;
} row;

unsigned int str_count = 1;
row *table;
long filesize;
char *mapped;


void Timeout() {
    printf("\nAll file:\n");

    for (int i = 0; i < table[str_count - 1].pos + table[str_count - 1].len; ++i) {
        putc(mapped[i], stdout);
    }
    putc('\n', stdout);

    _Exit(0);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        perror("run without arguments");
        return 0;
    }

    FILE *file = fopen(argv[1], "rb"); // Открываем файл в бинарном режиме
    if (file == NULL) {
        perror("can't open file");
        return 0;
    }
    fseek(file, 0, SEEK_END); // Перемещаем указатель в конец файла
    filesize = ftell(file); // Получаем позицию указателя (размер файла)
    fclose(file); // Закрываем файл

    int fdin = open(argv[1], O_RDONLY);
    mapped = mmap(0, filesize, PROT_READ, MAP_SHARED, fdin, 0);

    // get count of strings in file
    for (int i = 0; i < filesize; ++i) {
        if (mapped[i] == '\n') {
            ++str_count;
        }
    }

    table = malloc(sizeof(row) * str_count);

    // fill the table of offsets and lengths
    unsigned int cur_pos = 0;
    unsigned int cur = 0;
    unsigned int cur_str_len = 0;

    for (int i = 0; i < filesize; ++i) {
        if (mapped[i] == '\n') {
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

        for (int i = 0; i < table[n - 1].len; ++i) {
            putc(mapped[table[n - 1].pos + i], stdout);
        }

        printf("\nEnter string number: ");
        alarm(5);
    }
    putc('\n', stdout);

    free(table);
    return 0;
}
