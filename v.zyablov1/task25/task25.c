#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

// Функция для записи текста в канал
void write_to_pipe(int write_fd, const char *text) {
    if (write(write_fd, text, strlen(text)) == -1) {
        perror("write error");
        exit(1);
    }
    close(write_fd);  // Закрываем конец канала после записи
}

// Функция для чтения текста из канала и преобразования в верхний регистр
void read_and_transform_pipe(int read_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';

        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        printf("Transformed text: \n%s", buffer);
    } else if (bytes_read == -1) {
        perror("read error");
    }

    close(read_fd);  // Закрываем конец канала после чтения
}

// Основная программа
int main() {
    int pipe_fd[2];
    pid_t child_pid;

    // Создаем канал
    if (pipe(pipe_fd) == -1) {
        perror("pipe error");
        exit(1);
    }

    // Создаем дочерний процесс
    if ((child_pid = fork()) == -1) {
        perror("fork error");
        exit(1);
    }

    if (child_pid == 0) {
        // Дочерний процесс: пишет в канал
        const char *text = "Hello World! This is a test of the PIPE.\n";
        write_to_pipe(pipe_fd[1], text);
        exit(0);
    } else {
        // Родительский процесс: читает из канала и преобразует текст
        read_and_transform_pipe(pipe_fd[0]);
        wait(NULL);  // Ожидаем завершения дочернего процесса
    }

    return 0;
}

