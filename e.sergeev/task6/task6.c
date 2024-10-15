#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int fd;
off_t *offsets = NULL;
size_t *lengths = NULL;
int line_count = 0;
ssize_t bytes_read;

void print_all(int al){
	printf("\nTime is up!\n");
	lseek(fd, 0, SEEK_SET);
	char buffer[BUFFER_SIZE];
	while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
		write(STDOUT_FILENO, buffer, bytes_read);
	close(fd);
    free(offsets);
    free(lengths);

    exit(0);
}

int main() {
    char filename[] = "text.txt";
    char buffer[BUFFER_SIZE];
    signal(SIGALRM, print_all);

    if ((fd = open(filename, O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }

    off_t current_offset = 0;
    offsets = (off_t *)malloc(sizeof(off_t));
    lengths = (size_t *)malloc(sizeof(size_t));

    if (offsets == NULL || lengths == NULL) {
        perror("Memory allocation error");
        close(fd);
        return 1;
    }

    offsets[0] = 0;
    size_t current_length = 0;
	printf("lengths	offsets\n");
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                line_count++;
                current_length++;
                
                offsets = realloc(offsets, (line_count + 1) * sizeof(off_t));
                lengths = realloc(lengths, line_count * sizeof(size_t));

                if (offsets == NULL || lengths == NULL) {
                    perror("Memory allocation error");
                    close(fd);
                    return 1;
                }

                lengths[line_count - 1] = current_length;
                offsets[line_count] = current_offset + i + 1;
                printf("%d %d\n", lengths[line_count - 1], offsets[line_count]);
                current_length = 0;
            } 
            else current_length++;
        }
        current_offset += bytes_read;
    }

    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        free(offsets);
        free(lengths);
        return 1;
    }

    int line_number;
    char line_str[1024];
    while (1) {
        printf("Enter line number (0 to exit): ");
        alarm(5);
        scanf("%s", line_str);
        line_number = atoi(line_str);
		alarm(0);
        if (line_number == 0) break;
        else if (line_number > 0 && line_number <= line_count) {
            lseek(fd, offsets[line_number - 1], SEEK_SET);
            size_t len = lengths[line_number - 1];
            char *line = (char *)malloc(len + 1);

            if (line == NULL) {
                perror("Memory allocation error");
                break;
            }

            if (read(fd, line, len) != len) {
                perror("Error reading line");
                free(line);
                break;
            }

            line[len] = '\0';
            printf("Line %d: %s", line_number, line);
            free(line);
        } 
        else printf("Invalid line number.\n");
    }

    close(fd);
    free(offsets);
    free(lengths);

    return 0;
}
