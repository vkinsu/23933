#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

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

    off_t line_positions[MAX_LINES];
    int line_lengths[MAX_LINES];
    int line_count = 0;

    char c;
    off_t position = 0;
    while (read(fd, &c, 1) == 1) {
        if (line_count < MAX_LINES) {
            if (c == '\n') {
                line_positions[line_count] = position + 1;
                line_lengths[line_count] = 0;
                line_count++;
            }else{
                if (line_count == 0) {
                    line_positions[line_count] = position;
                    line_lengths[line_count] = 0;
                    line_count++;
                }
            }
        }
        position++;
    }

    lseek(fd, 0, SEEK_SET);
    for (int i = 0; i < line_count; i++) {
            while (read(fd, &c, 1) == 1 && c != '\n') {
                line_lengths[i]++;
            }
    }

    
    printf("String positions and lengths:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Position = %ld, Length = %d\n", i+1, line_positions[i], line_lengths[i]);
    }


    int line_number;
    while (1) {
        printf("Input number of line (0 to exit): ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > line_count) {
            printf("Error input. Try again\n");
            continue;
        }

        lseek(fd, line_positions[line_number - 1], SEEK_SET);
        char line[MAX_LINE_LENGTH];
        read(fd, line, line_lengths[line_number - 1]);
        line[line_lengths[line_number - 1]] = '\0'; 
        printf("Line %d: %s\n", line_number, line);
    }

    close(fd);
    return 0;
}
