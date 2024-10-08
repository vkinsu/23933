#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct {
    off_t offset; // cмещение от начала файла
    size_t length; 
} LineInfo;

void build_index_table(int fd, LineInfo **index, int *line_count) {
    char buffer[BUFFER_SIZE];
    int bytes_read, i, start = 0;
    off_t current_offset = 0;
    *line_count = 0;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                (*line_count)++;
                *index = (LineInfo*)realloc(*index, (*line_count) * sizeof(LineInfo));
                (*index)[*line_count - 1].offset = current_offset + start;
                (*index)[*line_count - 1].length = i - start + 1;
                start = i + 1; // обновляем начало следующей строки
            }
        }
        current_offset += bytes_read;

        // обработка последней части буфера
        if (start < bytes_read) {
            (*line_count)++;
            *index = (LineInfo*)realloc(*index, (*line_count) * sizeof(LineInfo));
            (*index)[*line_count - 1].offset = current_offset - bytes_read + start;
            (*index)[*line_count - 1].length = bytes_read - start + 1;
        }
    }

    // Отладочный вывод для печати таблицы индексов
    for (int j = 0; j < *line_count; j++) {
        printf("Line %d: Offset = %ld, Length = %zu\n", j + 1, (*index)[j].offset, (*index)[j].length);
    }
}

void print_line(int file_descript, LineInfo *index, int line_number) {
    char *buffer;
    lseek(file_descript, index[line_number].offset, SEEK_SET);
    buffer = malloc(index[line_number].length + 1);
    read(file_descript, buffer, index[line_number].length);
    buffer[index[line_number].length - 1] = '\0';
    printf("%s\n", buffer);
    free(buffer);
}

void debug_print_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            printf("Char: %c, Code: %d\n", buffer[i], buffer[i]);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    int file_descript;
    LineInfo *index = NULL;
    int line_count = 0, line_number;

    if ((file_descript = open(argv[1], O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    debug_print_file(argv[1]);

    build_index_table(file_descript, &index, &line_count);

    printf("Enter line number (0 to quit): ");
    while (scanf("%d", &line_number) == 1 && line_number != 0) {
        if (line_number > 0 && line_number <= line_count) {
            print_line(file_descript, index, line_number - 1);
        } else {
            printf("Invalid line number.\n");
        }
        printf("Enter line number (0 to quit): ");
    }

    free(index);
    close(file_descript);

    return 0;
}