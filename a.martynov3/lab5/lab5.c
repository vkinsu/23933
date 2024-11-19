#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long offset;
    long length;
} LineInfo;

// Функция для построения таблицы смещений строк
int build_offset_table(FILE *file, LineInfo **lines) {
    char line_buffer[1024]; // Буфер для строки (максимум 1024 символа)
    long offset = 0;
    int line_count = 0;

    *lines = (LineInfo *)malloc(sizeof(LineInfo) * 100);
    if (*lines == NULL) {
        perror("Memory allocation error");
        exit(1);
    }

    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        size_t line_length = strlen(line_buffer);

        // Убираем символ новой строки, если он есть
        if (line_length > 0 && line_buffer[line_length - 1] == '\n') {
            line_buffer[line_length - 1] = '\0';  // Убираем '\n' для подсчёта длины
        } else {
            line_length++; // Если нет '\n', учтём конец строки в подсчёте
        }

        // Увеличиваем таблицу по мере необходимости
        if (line_count % 100 == 0) {
            *lines = (LineInfo *)realloc(*lines, sizeof(LineInfo) * (line_count + 100));
            if (*lines == NULL) {
                perror("Memory reallocation error");
                exit(1);
            }
        }

        // Сохраняем смещение и длину строки
        (*lines)[line_count].offset = offset;
        (*lines)[line_count].length = line_length;
        offset += (line_length + 1); // +1 для символа новой строки
        line_count++;
    }

    return line_count;
}

// Функция для вывода строки по информации из таблицы
void print_line(FILE *file, LineInfo line) {
    char *buffer = (char *)malloc(line.length + 2); // +2 для '\n' и '\0'
    if (!buffer) {
        perror("Memory allocation error");
        exit(1);
    }

    if (fseek(file, line.offset, SEEK_SET) != 0) {
        perror("Error seeking in file");
        exit(1);
    }

    if (fread(buffer, 1, line.length, file) != line.length) { // Считываем ровно line.length символов
        perror("Error reading line");
        exit(1);
    }

    buffer[line.length] = '\0';  // Завершаем строку null-терминатором
    printf("%s", buffer);        // Выводим строку (не добавляем новую строку вручную, так как она есть в файле)
    free(buffer);
}

int main() {
    FILE *file;
    char filename[256];
    LineInfo *lines;
    int line_count;
    int line_number;

    printf("Enter the filename: ");
    scanf("%s", filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Строим таблицу смещений строк
    line_count = build_offset_table(file, &lines);

    // Выводим таблицу смещений строк
    printf("\nTable of line offsets and lengths:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Offset = %ld, Length = %ld\n", i + 1, lines[i].offset, lines[i].length);
    }

    // Чтение номеров строк и их вывод
    while (1) {
        printf("\nEnter line number (0 to quit): ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > line_count) {
            printf("Invalid line number. Please enter a number between 1 and %d.\n", line_count);
            continue;
        }

        // Выводим строку
        print_line(file, lines[line_number - 1]);

        // Выводим длину строки
        printf("\nLength of line %d: %ld\n", line_number, lines[line_number - 1].length);
    }

    free(lines);
    fclose(file);

    return 0;
}

