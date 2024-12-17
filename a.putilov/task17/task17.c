#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 40

// Управляющие символы
#define ERASE 127        // Символ удаления (обычно Backspace)
#define KILL 21          // CTRL-U для очистки строки
#define CTRL_W 23        // CTRL-W для удаления последнего слова
#define CTRL_D 4         // CTRL-D для завершения
#define BELL 7           // CTRL-G для звукового сигнала

// Функция для удаления последнего символа
void delete_last_char(char *line, int *len) {
    if (*len > 0) {
        (*len)--;
        printf("\b \b");  // Удаление символа с экрана
        fflush(stdout);
    }
}

// Функция для удаления последнего слова
void delete_last_word(char *line, int *len) {
    while (*len > 0 && isspace(line[*len - 1])) {
        delete_last_char(line, len);
    }
    while (*len > 0 && !isspace(line[*len - 1])) {
        delete_last_char(line, len);
    }
}

int main() {
    struct termios orig_termios, new_termios;
    char line[MAX_LINE_LENGTH + 1];
    int len = 0;

    // Настройка терминала: отключаем эхо и канонический режим
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    printf("Line editor started. Press CTRL-D at the beginning of a line to exit.\n");

    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == ERASE) {
            // Удаление последнего символа
            delete_last_char(line, &len);

        } else if (c == KILL) {
            // Удаление всей строки
            while (len > 0) {
                delete_last_char(line, &len);
            }

        } else if (c == CTRL_W) {
            // Удаление последнего слова
            delete_last_word(line, &len);

        } else if (c == CTRL_D) {
            // Завершение программы, если строка пуста
            if (len == 0) break;
            else printf("%c", BELL);

        } else if (!isprint(c) && c != '\n') {
            // Звуковой сигнал для непечатаемых символов
            printf("%c", BELL);
            fflush(stdout);

        } else if (isprint(c)) {
            // Печатаемые символы
            if (len < MAX_LINE_LENGTH) {
                line[len++] = c;
                putchar(c);
                fflush(stdout);
            } else {
                // Перенос слова на новую строку, если превышена длина
                putchar('\n');
                len = 0;
                line[len++] = c;
                putchar(c);
                fflush(stdout);
            }
        }
    }

    // Восстановление исходных настроек терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    printf("\nLine editor exited.\n");

    return 0;
}

