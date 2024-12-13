#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int pipe_fd[2]; // Дескрипторы канала: [0] для чтения, [1] для записи
    pid_t pid;
    char buffer[BUFFER_SIZE];
    // Создаем канал
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // Создаем дочерний процесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { 
        // Дочерний процесс (читатель, преобразователь текста)
        close(pipe_fd[1]); // Закрываем конец записи в канале
        // Чтение из канала
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytes_read] = '\0'; // Завершаем строку
            to_uppercase(buffer);     // Переводим в верхний регистр
            printf("Child received and transformed: %s\n", buffer);
        }
        if (bytes_read == -1) perror("read");
        close(pipe_fd[0]); // Закрываем конец чтения
        exit(EXIT_SUCCESS);
    } 
    else { 
        // Родительский процесс (писатель)
        close(pipe_fd[0]); // Закрываем конец чтения в канале
        // Ввод текста от пользователя
        printf("Enter text (Ctrl+D to finish):\n");
        while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
            size_t len = strlen(buffer);
            if (write(pipe_fd[1], buffer, len) == -1) {
                perror("write");
                close(pipe_fd[1]);
                exit(EXIT_FAILURE);
            }
        }
        close(pipe_fd[1]); // Закрываем конец записи
        wait(NULL);        // Ожидание завершения дочернего процесса
        printf("Parent: Finished sending text.\n");
    }
    return 0;
}

