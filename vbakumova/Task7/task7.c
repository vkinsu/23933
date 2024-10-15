#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFER_SIZE 1024

// Структура для хранения информации о строке
typedef struct {
    off_t position; // Позиция начала строки в файле
    size_t length;  // Длина строки
} LineInfo;

int fd;
char *mapped;
struct stat st;                     // Структура для получения информации о файле

void print_file_content(char *mapped, size_t filesize) {
    fwrite(mapped, 1, filesize, stdout); // Печатаем содержимое файла
}

void Alarm(int var) {
  print_file_content(mapped, st.st_size);
  printf("\n");
  munmap(mapped, st.st_size);
  close(fd);
  exit(0);
}

int main() {
    const char *filename = "text.txt"; // Имя файла для чтения
    LineInfo *lines = NULL;             // Массив для хранения информации о строках
    size_t line_count = 0;               // Счетчик строк

    // Открытие файла
    if ((fd = open(filename, O_RDONLY)) == -1) {
        perror("Ошибка открытия файла");
        return 1;
    }

    // Получаем информацию о файле
    if (fstat(fd, &st) == -1) {
        perror("Ошибка получения информации о файле");
        close(fd);
        return 1;
    }

    // Отображение файла в память
    mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Ошибка отображения файла в память");
        close(fd);
        return 1;
    }

    // Инициализация начального массива для хранения информации о строках
    lines = (LineInfo *)malloc(sizeof(LineInfo));
    if (lines == NULL) {
        perror("Ошибка выделения памяти");
        munmap(mapped, st.st_size); // Освобождение отображения
        close(fd);
        return 1;
    }

    off_t current_offset = 0; // Текущая позиция в файле
    size_t current_length = 0; // Текущая длина строки

    // Построение таблицы отступов и длин строк
    for (size_t i = 0; i < st.st_size; i++) {
        if (mapped[i] == '\n') { // Найден перевод строки
            lines[line_count].length = current_length + 1; // Длина строки с учетом символа новой строки
            lines[line_count].position = current_offset;

            // Увеличение размера массива, если необходимо
            line_count++;
            lines = (LineInfo *)realloc(lines, (line_count + 1) * sizeof(LineInfo));
            if (lines == NULL) {
                perror("Ошибка выделения памяти");
                munmap(mapped, st.st_size); // Освобождение отображения
                close(fd);
                return 1;
            }

            current_offset += current_length + 1; // Обновляем позицию
            current_length = 0; // Сбрасываем длину для следующей строки
        } else {
            current_length++;
        }
    }

    // Цикл для запроса номера строки
    int line_number;
    signal(SIGALRM, Alarm);
    bool flag = true;
    alarm(5);
    printf("Можнно ввести номер строки от 1 до %ld. Для выхода введите 0.\n", line_count);
    printf("Введите номер строки: ");

    while (scanf("%d", &line_number)) {
        alarm(0);

        // Пользователь ввел номер строки
        if (line_number < 0 || line_number > line_count) {
            fprintf(stderr, "Ошибка ввода. Пожалуйста, введите целое число от 0 до %ld: ", line_count);
            continue; // Игнорируем некорректный ввод
        }

        if (line_number == 0) {
            break; // Выход из программы
        } else if (line_number > 0 && line_number <= line_count) {
            // Печатаем строку, используя отображение в память
            size_t len = lines[line_number - 1].length; // Длина строки
            //printf("Строка %d: ", line_number);
            fwrite(mapped + lines[line_number - 1].position, 1, len, stdout); // Вывод строки
        } else {
            printf("Неверный номер строки.\n"); // Обработка некорректного ввода
        }
        alarm(5);
        printf("Введите номер строки (0 для выхода): ");
    }

    // Освобождение ресурсов
    munmap(mapped, st.st_size); // Освобождение отображения
    close(fd);
    free(lines);

    return 0;
}
