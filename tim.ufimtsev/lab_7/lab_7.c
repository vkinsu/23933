#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct {
    off_t offset; // cмещение от начала файла
    size_t length;
} LineInfo;

void build_index_table(const char *data, size_t size, LineInfo **index, int *line_count) {
    off_t start = 0;
    *line_count = 0;

    for (off_t i = 0; i < size; i++) {
        if (data[i] == '\n') {
            (*line_count)++;
            *index = (LineInfo*)realloc(*index, (*line_count) * sizeof(LineInfo));
            (*index)[*line_count - 1].offset = start;
            (*index)[*line_count - 1].length = i - start + 1;
            start = i + 1; // обновляем начало следующей строки
        }
    }

    if (start < size) {
        (*line_count)++;
        *index = (LineInfo*)realloc(*index, (*line_count) * sizeof(LineInfo));
        (*index)[*line_count - 1].offset = start;
        (*index)[*line_count - 1].length = size - start;
    }
}

void print_line(const char *data, LineInfo *index, int line_number) {
    size_t length = index[line_number].length;

    for (size_t i = index[line_number].offset; i < index[line_number].offset + length; i++) {
        putchar(data[i]);
    }
}

void print_all_lines(const char *data, size_t size) {
    fwrite(data, 1, size, stdout);
}

int main(int argc, char *argv[]) {

    const char *path = argv[1];
    int file_descript = open(path, O_RDONLY); 

    struct stat sb; // информация о файле
    fstat(file_descript, &sb);
    size_t size = sb.st_size;

    // отображение файла в память
    const char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, file_descript, 0);

    close(file_descript);

    LineInfo *index = NULL;
    int line_count = 0;
    build_index_table(data, size, &index, &line_count);

    while (1) {
        printf("Enter line number (0 to quit): ");
        fflush(stdout);

        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

        if (rv == 0) {
            printf("\nTimeout! Printing all lines:\n");
            print_all_lines(data, size);
            break;
        } else {
            int line_number;
            if (scanf("%d", &line_number) == 1 && line_number != 0) {
                if (line_number > 0 && line_number <= line_count) {
                    print_line(data, index, line_number - 1);
                if (line_number == line_count) {
                    putchar('\n');
                }
                } else {
                    printf("Invalid line number.\n");
                }
            } else {
                break;
            }
        }
    }

    munmap((void *)data, size);
    free(index);

    return 0;
}