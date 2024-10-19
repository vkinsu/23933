#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

typedef struct {
    off_t offset;   // Отступ в файле
    size_t length;  // Длина строки
} LineInfo;

void build_line_table(int fd, LineInfo *line_table, size_t *line_count) {
    char buffer[MAX_LINE_LENGTH];
    off_t offset = 0;
    ssize_t bytes_read;
    *line_count = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                line_table[*line_count].offset = offset;
                line_table[*line_count].length = i + 1; // включая '\n'
                (*line_count)++;
                offset += i + 1;

                // Проверка на максимальное количество строк
                if (*line_count >= MAX_LINES) {
                    return;
                }
            }
        }
        offset += bytes_read; // Обновляем общий отступ
    }
}

int main() {
    const char *filename = "file.txt"; // Имя файла для анализа
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Ошибка при открытии файла");
        return EXIT_FAILURE;
    }

    LineInfo line_table[MAX_LINES];
    size_t line_count;
    build_line_table(fd, line_table, &line_count);
    close(fd);

    // Вывод таблицы отступов и длин строк
    printf("Таблица отступов и длин строк:\n");
    for (size_t i = 0; i < line_count; i++) {
        printf("Строка %zu: Отступ = %lld, Длина = %zu\n", i, (long long)line_table[i].offset, line_table[i].length);
    }

    // Запрос номера строки
    int line_number;
    while (1) {
        printf("Введите номер строки (0 для выхода): ");
        scanf("%d", &line_number);
        
        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > line_count) {
            printf("Некорректный номер строки. Попробуйте снова.\n");
            continue;
        }

        // Открываем файл для чтения строки
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            perror("Ошибка при открытии файла");
            return EXIT_FAILURE;
        }

        // Позиционируемся на нужный отступ
        lseek(fd, line_table[line_number - 1].offset, SEEK_SET);
        
        // Читаем строку
        char line[MAX_LINE_LENGTH];
        read(fd, line, line_table[line_number - 1].length);
        line[line_table[line_number - 1].length - 1] = '\0'; // Заменяем '\n' на '\0'
        printf("Строка %d: %s\n", line_number, line);

        close(fd);
    }

    return EXIT_SUCCESS;
}

