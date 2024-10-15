#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

int time_up = 0;
void handle_alarm(int signum) {
    time_up = 1;
}

void print_file_content(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("error openning file");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    int last_character = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        last_character = (line[strlen(line) - 1] == '\n') ? 1 : 0;
    }

    if (last_character == 0) {
        printf("\n");
    }
    
    fclose(file);
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


    signal(SIGALRM, handle_alarm);
    int line_number;
    while (1) {
        printf("Input number of line (0 to exit): ");
        alarm(5);

        if (scanf("%d", &line_number) == 1) {
            alarm(0); 

            if (line_number == 0) {
                return 0;
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
        if (time_up == 1) {
            printf("\nFile:\n");
            print_file_content(argv[1]);
            return EXIT_SUCCESS;
        }
    }

    close(fd);
    return 0;
}
