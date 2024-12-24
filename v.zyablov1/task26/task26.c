#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main() {
    FILE *pipe;
    char buffer[BUFFER_SIZE];

    // Открываем конвейер с командой для преобразования в верхний регистр
    pipe = popen("tr a-z A-Z", "w");  // Отправляем команду 'tr' для преобразования в верхний регистр
    if (pipe == NULL) {
        perror("popen error");
        exit(1);
    }

    // Записываем текст в конвейер
    const char *text = "Hello World! This is a test of the PIPE.\n";
    fputs(text, pipe);

    // Закрываем конвейер
    if (pclose(pipe) == -1) {
        perror("pclose error");
        exit(1);
    }

    // Открываем конвейер для чтения результата
    pipe = popen("cat", "r");  // Читаем результат из канала через команду 'cat'
    if (pipe == NULL) {
        perror("popen error");
        exit(1);
    }

    // Читаем результат (преобразованный текст) и выводим
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("Transformed text: %s", buffer);
    }

    // Закрываем конвейер
    if (pclose(pipe) == -1) {
        perror("pclose error");
        exit(1);
    }

    return 0;
}

