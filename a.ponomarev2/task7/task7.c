#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

int time_up = 0;
void handle_alarm(int signum) {
    time_up = 1;
}

void print_file_content(const char *filename, char *data, size_t length) {
    if (length > 0 && data[length - 1] != '\n') {
        printf("%.*s\n", (int)length, data);
    } else {
        printf("%.*s", (int)length, data);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: ./task5.c <path>");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("error openning file");
        return 1;
    }


    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    char *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("error file to data");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    off_t line_positions[MAX_LINES];
    int line_lengths[MAX_LINES];
    int line_count = 0;

    for (off_t i = 0; i < sb.st_size; i++) {
        if (line_count < MAX_LINES) {
            if (data[i] == '\n') {
                line_positions[line_count] = i + 1; 
                line_count++;
            } else if (line_count == 0) {
                line_positions[line_count] = 0;
                line_count++;
            }
        }
    }

    for (int i = 0; i < line_count; i++) {
        if (i == line_count - 1) {
            line_lengths[i] = sb.st_size - line_positions[i];
        } else {
            line_lengths[i] = line_positions[i + 1] - line_positions[i] - 1;
        }
    }
  
    printf("String positions and lengths:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Position = %ld, Length = %d\n", i+1, line_positions[i], line_lengths[i]);
    }


    signal(SIGALRM, handle_alarm);
    int line_number;
    while (1) {
        printf("Input number of line (0 to exit): ");
        alarm(5);

        if (scanf("%d", &line_number) == 1) {
            alarm(0); 

            if (line_number == 0) {
                munmap(data, sb.st_size);
                return 0;
            }

            if (line_number < 1 || line_number > line_count) {
                printf("Error input. Try again\n");
                continue;
            }

        printf("Line %d: %.*s\n", line_number, line_lengths[line_number - 1], data + line_positions[line_number - 1]);
        }
        if (time_up == 1) {
            printf("\nFile:\n");
            print_file_content(argv[1], data, sb.st_size);
            munmap(data, sb.st_size);
            return EXIT_SUCCESS;
        }
    }

    munmap(data, sb.st_size);
    return 0;
}
