#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2]; // Дескрипторы для канала (pipe)
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Создаем канал
    if (pipe(pipefd) == -1) {
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
        // Дочерний процесс
        close(pipefd[1]); // Закрываем конец для записи

        // Читаем данные из канала
        ssize_t count = read(pipefd[0], buffer, BUFFER_SIZE);
        if (count == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffer[count] = '\0'; // Завершаем строку

        // Преобразуем в верхний регистр
        for (int i = 0; i < count; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        // Выводим результат
        printf("Modified text: %s\n", buffer);

        close(pipefd[0]); // Закрываем конец для чтения
    } else { 
        // Родительский процесс
        close(pipefd[0]); // Закрываем конец для чтения

        // Текст для отправки
        const char *text = "Hello, World! MixedCase123";
        printf("Original text: %s\n", text);

        // Пишем данные в канал
        if (write(pipefd[1], text, strlen(text)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]); // Закрываем конец для записи
        wait(NULL);       // Ожидаем завершения дочернего процесса
    }

    return 0;
}
