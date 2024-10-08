#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int fd;
off_t *offsets = NULL;
size_t *lengths = NULL;
int line_count = 0;
char *mapped_file = NULL;
size_t file_size = 0;

void handle_alarm(int sig) {
    printf("\nTime's up! Printing entire file content:\n");
    write(STDOUT_FILENO, mapped_file, file_size);
    close(fd);
    free(offsets);
    free(lengths);
    munmap(mapped_file, file_size);
    exit(0);
}

int main() {
    char filename[] = "text.txt";
    struct stat file_stat;

    signal(SIGALRM, handle_alarm);

    if ((fd = open(filename, O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }

    if (fstat(fd, &file_stat) == -1) {
        perror("Error getting file stats");
        close(fd);
        return 1;
    }
    file_size = file_stat.st_size;

    mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_file == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    offsets = (off_t *)malloc(sizeof(off_t));
    lengths = (size_t *)malloc(sizeof(size_t));

    if (offsets == NULL || lengths == NULL) {
        perror("Memory allocation error");
        close(fd);
        munmap(mapped_file, file_size);
        return 1;
    }

    offsets[0] = 0;
    size_t current_length = 0;

    for (off_t i = 0; i < file_size; i++) {
        if (mapped_file[i] == '\n') {
            line_count++;
            current_length++;

            offsets = realloc(offsets, (line_count + 1) * sizeof(off_t));
            lengths = realloc(lengths, line_count * sizeof(size_t));

            if (offsets == NULL || lengths == NULL) {
                perror("Memory allocation error");
                close(fd);
                munmap(mapped_file, file_size);
                return 1;
            }

            lengths[line_count - 1] = current_length;
            offsets[line_count] = i + 1;
            current_length = 0;
        }
        else current_length++;
    }
    int line_number;
    while (1) {
        printf("Enter line number (0 to exit): ");
        alarm(5);
        scanf("%d", &line_number);
        alarm(0);

        if (line_number == 0)break;
        else if (line_number > 0 && line_number <= line_count) {
            off_t start = offsets[line_number - 1];
            size_t len = lengths[line_number - 1];
            printf("Line %d: %.*s", line_number, (int)len, mapped_file + start);
        } 
        else printf("Invalid line number.\n");
    }
    
    close(fd);
    free(offsets);
    free(lengths);
    munmap(mapped_file, file_size);

    return 0;
}

