#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE_LEN 40
#define ERASE 127       // Символ забоя (Backspace)
#define KILL '\025'     // CTRL-U
#define CTRL_W '\027'   // CTRL-W
#define CTRL_D '\004'   // CTRL-D
#define CTRL_G '\007'   // CTRL-G

void set_terminal_mode(int enable) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    if (enable) {
        term.c_lflag |= (ICANON | ECHO); // Включаем канонический режим и эхо
    } else {
        term.c_lflag &= ~(ICANON | ECHO); // Выключаем канонический режим и эхо
        term.c_cc[VMIN] = 1;  // Считывать по 1 символу
        term.c_cc[VTIME] = 0; // Без тайм-аутов
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void beep() {
    write(STDOUT_FILENO, "\a", 1);
}

void print_line(const char *line, int len) {
    write(STDOUT_FILENO, "\r", 1);       // Переместить курсор в начало строки
    write(STDOUT_FILENO, line, len);    // Печать строки
    write(STDOUT_FILENO, " ", 1);       // Удалить хвост
    for (int i = len; i <= MAX_LINE_LEN; i++) {
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "\r", 1);       // Возврат курсора
    write(STDOUT_FILENO, line, len);    // Показать строку
}

int main() {
    char line[MAX_LINE_LEN + 1] = {0}; // Текущая строка
    int len = 0;                      // Текущая длина строки

    set_terminal_mode(0); // Выключить канонический режим и эхо
    printf("Input mode (CTRL-D to exit):\n");

    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) {
            break;
        }

        if (c == CTRL_D && len == 0) {
            break; // Выход, если CTRL-D на пустой строке
        } else if (c == ERASE) {
            if (len > 0) {
                len--; // Удалить последний символ
                line[len] = '\0';
            }
        } else if (c == KILL) {
            len = 0; // Очистить строку
            line[0] = '\0';
        } else if (c == CTRL_W) {
            // Удалить последнее слово
            while (len > 0 && line[len - 1] == ' ') {
                len--;
            }
            while (len > 0 && line[len - 1] != ' ') {
                len--;
            }
            line[len] = '\0'; // Обновить строку
        } else if (c >= 32 && c <= 126) { // Печатаемые символы
            if (len < MAX_LINE_LEN) {
                line[len++] = c;
            } else {
                beep(); // Звуковой сигнал, если строка переполнена
            }
        } else {
            beep(); // Звуковой сигнал для непечатаемых символов
        }

        line[len] = '\0'; // Обновить строку
        print_line(line, len);
    }

    set_terminal_mode(1); // Восстановить канонический режим и эхо
    printf("\nExiting...\n");
    return 0;
}
