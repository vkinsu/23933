#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>


void print(char *input_line) {
    printf("\r%s", input_line);
    fflush(stdout);
}


int main() {
    struct termios orig_termios;
    char input_line[40 + 1][40 + 1] = {0}; // 40 lines 40 chars
    int len = 0;
    int line = 0;

    // enable raw mode
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);

    while (1) {
        print(input_line[line]);

        char c = (char) getchar();

        if (c == 127) { // ERASE (Backspace)
            if (len > 0) {
                input_line[line][--len] = '\0';
                input_line[line][len] = ' ';

                print(input_line[line]);

                input_line[line][len] = '\0';

            } else if (line > 0) {
                --line;
                len = 40;

                print("\033[A");

            } else {
                write(1,"\a",1);
            }

        } else if (c == 0x15) { // KILL (CTRL+U)
            if (len == 0) {
                write(1,"\a",1);

            } else {
                for (int i = 0; i < 40; ++i) {
                    input_line[line][i] = ' ';
                }

                print(input_line[line]);

                len = 0;
                input_line[line][0] = '\0';
            }

        } else if (c == 0x17) { // CTRL-W
            while (len > 0 && input_line[line][len - 1] != ' ') {
                len--;
                input_line[line][len] = ' ';
            }
            while (len > 0 && input_line[line][len - 1] == ' ') {
                len--;
            }

            print(input_line[line]);

            input_line[line][len] = '\0';

        } else if (c == 0x04) { // CTRL-D
            if (len == 0 && line == 0) {
                break;
            } else {
                write(1,"\a",1);
            }

        } else if (isprint(c)) { //print
            if (len < 40) {
                input_line[line][len++] = c;
                input_line[line][len] = '\0';

            } else if (line < 40) {
                ++line;
                len = 0;

                printf("\n");
                fflush(stdout);

                input_line[line][len++] = c;
                input_line[line][len] = '\0';

            }

        } else {
            write(1,"\a",1);
        }

    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // disable raw mode
    printf("\n");

    return 0;
}
