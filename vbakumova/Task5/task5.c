#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

// Структура для хранения информации о строке
typedef struct {
    off_t position; // Позиция начала строки в файле
    size_t length;  // Длина строки
} LineInfo;

int main() {
    int fd;
    const char *filename = "text.txt"; // Имя файла для чтения
    LineInfo *lines = NULL;             // Массив для хранения информации о строках
    char buffer[BUFFER_SIZE];            // Буфер для чтения файла
    int line_count = 0;                  // Счетчик строк
    ssize_t bytes_read;                  // Количество байт, прочитанных из файла

    // Открытие файла
    if ((fd = open(filename, O_RDONLY)) == -1) {
        perror("Ошибка открытия файла");
        return 1;
    }

    // Инициализация начального массива для хранения информации о строках
    lines = (LineInfo *)malloc(sizeof(LineInfo));
    if (lines == NULL) {
        perror("Ошибка выделения памяти");
        close(fd);
        return 1;
    }

    off_t current_offset = 0; // Текущая позиция в файле
    size_t current_length = 0; // Текущая длина строки

    // Чтение файла и построение таблицы отступов и длин строк
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') { // Найден перевод строки
                lines[line_count].length = current_length + 1; // Длина строки с учетом символа новой строки
                lines[line_count].position = current_offset;

                // Увеличение размера массива, если необходимо
                line_count++;
                lines = (LineInfo *)realloc(lines, (line_count + 1) * sizeof(LineInfo));
                if (lines == NULL) {
                    perror("Ошибка выделения памяти");
                    close(fd);
                    return 1;
                }

                current_offset += current_length + 1; // Обновляем позицию
                current_length = 0; // Сбрасываем длину для следующей строки
            } else {
                current_length++;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Ошибка чтения файла");
        close(fd);
        free(lines);
        return 1;
    }

    // Цикл для запроса номера строки
    int line_number;
    printf("Можнно ввести номер строки от 1 до %d. Для выхода введите 0.\nВведите номер строки: ", line_count);
    while (scanf("%d", &line_number)) {
        if (line_number < 0 || line_number > line_count) {
            fprintf(stderr, "Ошибка ввода. Пожалуйста, введите целое число от 0 до %d: ", line_count);
            continue; // Игнорируем некорректный ввод
        }

        if (line_number == 0) {
            break; // Выход из программы
        } else if (line_number > 0 && line_number <= line_count) {
            // Позиционируемся на начало нужной строки
            if (lseek(fd, lines[line_number - 1].position, SEEK_SET) == -1) {
                perror("Ошибка позиционирования в файле");
                break;
            }

            size_t len = lines[line_number - 1].length; // Длина строки
            char *line = (char *)malloc(len + 1); // Выделение памяти для строки

            if (line == NULL) {
                perror("Ошибка выделения памяти");
                break;
            }

            // Чтение строки
            if (read(fd, line, len) != len) {
                perror("Ошибка чтения строки");
                free(line);
                break;
            }

            line[len] = '\0'; // Завершение строки нулевым символом
            printf("%s", line); // Вывод строки
            free(line);
        } else {
            printf("Неверный номер строки.\n"); // Обработка некорректного ввода
        }
        printf("Введите номер строки (0 для выхода): ");
    }

    // Освобождение ресурсов
    close(fd);
    free(lines);

    return 0;
}
