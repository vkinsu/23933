#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define ERASE 127
#define KILL 21
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7
#define MAX_LEN 40

void set_terminal_mode(struct termios *original_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, original_termios);
    new_termios = *original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void reset_terminal_mode(struct termios *original_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_termios);
}

void print_bell() {
    putchar(CTRL_G);
    fflush(stdout);
}

void handle_input() {
    char buffer[MAX_LEN + 1];
    int len = 0;
    int c;

    while (1) {
        c = getchar();

        if (c == CTRL_D && len == 0) {
            break;
        }

        if (c >= 32 && c <= 126) {
            if (len < MAX_LEN) {
                buffer[len++] = c;
                putchar(c);
                fflush(stdout);
            } else {
                putchar('\n');
                buffer[0] = c;
                len = 1;
                putchar(c);
                fflush(stdout);
            }
        } else if (c == ERASE) {
            if (len > 0) {
                len--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == KILL) {
            while (len > 0) {
                len--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == CTRL_W) {
            while (len > 0 && (buffer[len-1] == ' ' || buffer[len-1] == '\t')) {
                len--;
                printf("\b \b");
                fflush(stdout);
            }
            while (len > 0 && buffer[len-1] != ' ' && buffer[len-1] != '\t') {
                len--;
                printf("\b \b");
                fflush(stdout);
            }
        } else {
            print_bell();
        }
    }
}

int main() {
    struct termios original_termios;

    set_terminal_mode(&original_termios);

    printf("Custom terminal input mode. Press CTRL-D at the beginning of a line to exit.\n");
    handle_input();

    reset_terminal_mode(&original_termios);

    printf("\nExited custom terminal input mode.\n");
    return 0;
}
