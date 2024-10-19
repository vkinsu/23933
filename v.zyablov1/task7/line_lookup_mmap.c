#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

#define MAX_LINES 100

typedef struct {
    off_t offset;   // Отступ в файле
    size_t length;  // Длина строки
} LineInfo;

// Флаг для указания на истечение времени
volatile sig_atomic_t time_expired = 0;

void handle_alarm(int sig) {
    time_expired = 1; // Устанавливаем флаг истечения времени
}

void build_line_table(char *mapped, size_t file_size, LineInfo *line_table, size_t *line_count) {
    off_t offset = 0;
    *line_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (mapped[i] == '\n') {
            line_table[*line_count].offset = offset;
            line_table[*line_count].length = i - offset + 1; // включая '\n'
            (*line_count)++;
            offset = i + 1;

            // Проверка на максимальное количество строк
            if (*line_count >= MAX_LINES) {
                return;
            }
        }
    }
}

void print_file_contents(char *mapped, size_t file_size) {
    write(STDOUT_FILENO, mapped, file_size);
}

int main() {
    const char *filename = "file.txt"; // Имя файла для анализа
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Ошибка при открытии файла");
        return EXIT_FAILURE;
    }

    // Получаем размер файла
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // Возвращаем указатель в начало

    // Отображаем файл в память
    char *mapped = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Ошибка при отображении файла в память");
        close(fd);
        return EXIT_FAILURE;
    }

    LineInfo line_table[MAX_LINES];
    size_t line_count;
    build_line_table(mapped, file_size, line_table, &line_count);
    close(fd);

    // Вывод таблицы отступов и длин строк
    printf("Таблица отступов и длин строк:\n");
    for (size_t i = 0; i < line_count; i++) {
        printf("Строка %zu: Отступ = %lld, Длина = %zu\n", i, (long long)line_table[i].offset, line_table[i].length);
    }

    // Установка обработчика сигнала для alarm
    signal(SIGALRM, handle_alarm);
    alarm(5); // Устанавливаем таймер на 5 секунд

    // Запрос номера строки
    int line_number;
    printf("Введите номер строки (0 для выхода): ");
    while (!time_expired) {
        if (scanf("%d", &line_number) == 1) {
            if (line_number == 0) {
                break;
            }

            if (line_number < 1 || line_number > line_count) {
                printf("Некорректный номер строки. Попробуйте снова.\n");
                continue;
            }

            // Печатаем нужную строку
            printf("Строка %d: ", line_number);
            write(STDOUT_FILENO, mapped + line_table[line_number - 1].offset, line_table[line_number - 1].length);
            alarm(5); // Сбрасываем таймер
            printf("Введите номер строки (0 для выхода): ");
        } else {
            // Ввод не был распознан
            printf("Некорректный ввод. Попробуйте снова.\n");
            // Сбрасываем таймер
            alarm(5);
            printf("Введите номер строки (0 для выхода): ");
        }
    }

    // Если время истекло, выводим содержимое файла
    if (time_expired) {
        printf("Время истекло! Выводим содержимое файла:\n");
        print_file_contents(mapped, file_size);
    }

    // Освобождаем память
    munmap(mapped, file_size);

    return EXIT_SUCCESS;
}

