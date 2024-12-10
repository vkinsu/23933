#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ERASE '#'
#define KILL '@'
#define CTRL_W 0x17
#define CTRL_D 0x04
#define BEL "\a"

int main() {
    struct termios old_termios, new_termios;
    char line[41] = { 0 };
    int line_len = 0;
    int cursor_pos = 0;

    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);


    int i, start, len;
    while (1) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) <= 0) break;

        switch (ch) {
            case ERASE:
                if (cursor_pos > 0) {
                    cursor_pos--;
                    line[cursor_pos] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                    line_len--;
                }
                break;
            case KILL:
                printf("\r\033[K");
                fflush(stdout);
                line_len = 0;
                cursor_pos = 0;
                memset(line, 0, sizeof(line));
                break;
            case CTRL_W:
                i = cursor_pos - 1;
                while (i >= 0 && line[i] == ' ') i--;
                while (i >= 0 && line[i] != ' ') i--;
                start = i + 1;
                len = cursor_pos - start;

                if (len > 0) {
                    printf("\033[%dD", len);
                    printf("\033[K");
                    fflush(stdout);

                    memmove(&line[start], &line[cursor_pos], line_len - cursor_pos + 1);
                    line_len -= len;
                    cursor_pos = start;
                    line[line_len] = '\0';

                }
                break;
            case CTRL_D:
                if (cursor_pos == 0) goto exit;
                break;
            case '\n':
                printf("\n");
                line_len = 0;
                cursor_pos = 0;
                memset(line, 0, sizeof(line));
                break;
            default:
                if (isprint(ch)) {
                    if (line_len < 40) {
                        line[line_len++] = ch;
                        line[line_len] = '\0';
                        printf("%c", ch);
                        fflush(stdout);
                        cursor_pos++;
                    } 
                    else printf(BEL);
                }
                else printf(BEL);
                break;
        }
    }

exit:
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return 0;
}

