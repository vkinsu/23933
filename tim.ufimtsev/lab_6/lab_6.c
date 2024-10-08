#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

typedef struct {
    off_t offset; // cмещение от начала файла
    size_t length;
} LineInfo;

void build_index_table(int file_descript, LineInfo **index, int *line_count) {
    char buffer[BUFFER_SIZE];
    int bytes_read, i, start = 0;
    off_t current_offset = 0;
    *line_count = 0;

    while ((bytes_read = read(file_descript, buffer, BUFFER_SIZE)) > 0) {
        for (i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                (*line_count)++;
                *index = (LineInfo*)realloc(*index, (*line_count) * sizeof(LineInfo));
                (*index)[*line_count - 1].offset = current_offset + start;
                (*index)[*line_count - 1].length = i - start + 1;
                start = i + 1; 
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
}

void print_line(int file_descript, LineInfo *index, int line_number) {
    char *buffer;
    lseek(file_descript, index[line_number].offset, SEEK_SET);
    buffer = (char*)malloc(index[line_number].length + 1);
    read(file_descript, buffer, index[line_number].length);
    buffer[index[line_number].length - 1] = '\0';
    printf("%s\n", buffer);
    free(buffer);
}

void print_all_lines(int file_descript) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    lseek(file_descript, 0, SEEK_SET);

    while ((bytes_read = read(file_descript, buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
}

int main(int argc, char *argv[]) {
    int file_descript;
    LineInfo *index = NULL;
    int line_count = 0, line_number;

    if ((file_descript = open(argv[1], O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    build_index_table(file_descript, &index, &line_count);

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
            print_all_lines(file_descript);
            break;
        } else {
            if (scanf("%d", &line_number) == 1 && line_number != 0) {
                if (line_number > 0 && line_number <= line_count) {
                    print_line(file_descript, index, line_number - 1);
                } else {
                    printf("Invalid line number.\n");
                }
            } else {
                break;
            }
        }
    }

    free(index);
    close(file_descript);

    return 0;
}