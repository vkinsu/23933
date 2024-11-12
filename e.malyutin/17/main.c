#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "unicode.h"
#include "vec.h"

#define ESC "\033["

const u64 MAX_WIDTH = 40;

u8 *text = NULL;
u64 text_pos = 0;

u64 max_lines = 0;

u64 cx = 0, cy = 0;

void flush() {
    fflush(stdout);
}

void put(u8 *str, usize len) {
    usize pos = 0;
    while (pos < len) {
        if (str[pos] == '\n') {
            cy++;
            cx = 0;
        } else {
            cx += uc_width(&str[pos]);
        }
        pos += uc_char_len(str[pos]);
    }
    if (cy + 1 > max_lines) {
        max_lines = cy + 1;
    }
    fwrite(str, len, 1, stdout);
}

void cursor_to(u64 x, u64 y) {
    i64 ix = x - cx;
    i64 iy = y - cy;

    if (ix < 0) {
        printf(ESC "%ldD", -ix);
    } else if (ix > 0) {
        printf(ESC "%ldC", ix);
    }

    if (iy < 0) {
        printf(ESC "%ldA", -iy);
    } else if (iy > 0) {
        printf(ESC "%ldB", iy);
    }

    cx = x;
    cy = y;
}

void clear() {
    for (usize i = 0; i < max_lines; i++) {
        cursor_to(0, i);
        printf(ESC "2K");
    }
    cursor_to(0, 0);
}

void redraw() {
    clear();

    u64 rx = 0, ry = 0;
    usize pos = 0;

    while (pos < vec_len(text)) {
        u8 clen = uc_char_len(text[pos]);
        u8 width = uc_code_width(uc_decode_char(clen, &text[pos]));

        if (pos == text_pos) {
            rx = cx;
            ry = cy;
        }

        if (text[pos] != '\n' && cx + width > MAX_WIDTH) {
            put((u8 *)"\n", 1);
        }

        put(&text[pos], clen);

        pos += clen;
    }

    if (text_pos < vec_len(text)) {
        cursor_to(rx, ry);
    }

    flush();
}

void on_erase() {
    if (text_pos == 0) {
        return;
    }

    u8 *prev = uc_prev_char(&text[text_pos - 1]);
    u8 diff = &text[text_pos - 1] - prev + 1;
    vec_remove(text, text_pos - diff, diff);
    text_pos -= diff;

    redraw();
}

void on_left() {
    if (text_pos == 0) {
        return;
    }

    u8 *prev = uc_prev_char(&text[text_pos - 1]);
    u8 diff = &text[text_pos - 1] - prev + 1;
    text_pos -= diff;

    redraw();
}

void on_right() {
    if (text_pos >= vec_len(text)) {
        return;
    }

    text_pos += uc_char_len(text[text_pos]);

    redraw();
}

void on_insert(u8 *input, usize len) {
    vec_insert(text, text_pos, input, len);
    text_pos += len;
    redraw();
}

u64 prev_word() {
    usize start = text_pos - 1;

    while (start > 0 && text[start] == ' ') {
        start--;
    }

    while (start > 0 && text[start] != ' ') {
        start--;
    }

    return start;
}

void on_prev_word() {
    if (text_pos == 0) {
        return;
    }

    text_pos = prev_word();
    redraw();
}

u64 next_word() {
    usize start = text_pos + 1;

    while (start < vec_len(text) && text[start] == ' ') {
        start++;
    }

    while (start < vec_len(text) && text[start] != ' ') {
        start++;
    }

    return start;
}

void on_next_word() {
    if (text_pos >= vec_len(text)) {
        return;
    }

    text_pos = next_word();
    redraw();
}

void on_erase_word() {
    if (text_pos == 0) {
        return;
    }

    u64 prev = prev_word();

    if (text_pos - prev > 1) {
        vec_remove(text, prev, text_pos - prev);
        text_pos = prev;
    }

    redraw();
}

void on_line_start() {
    text_pos = 0;
    redraw();
}

void on_line_end() {
    text_pos = vec_len(text);
    redraw();
}

void on_clear() {
    vec_free(text);
    text_pos = 0;
    redraw();
}

void beep() {
    printf("\007");
    flush();
}

int main(int argc, char **argv) {
    struct termios term_old;
    tcgetattr(fileno(stdin), &term_old);

    struct termios term = term_old;

    term.c_lflag &= ~ECHO & ~ICANON & ~ISIG;
    tcsetattr(fileno(stdin), 0, &term);

    while (1) {
        int c = fgetc(stdin);

        if (c == -1) {
            break;
        } else if (c == 1) {
            on_line_start();
        } else if (c == 5) {
            on_line_end();
        } else if (c == 3) {
            on_clear();
        } else if (c == 4) {
            if (text_pos == 0) {
                break;
            } else {
                beep();
            }
        } else if (c == 23) {
            on_erase_word();
        } else if (c == 27) {
            if ((c = fgetc(stdin)) != '[') {
                beep();
                continue;
            }

            if ((c = fgetc(stdin)) == 'D') {
                on_left();
                continue;
            } else if (c == 'C') {
                on_right();
                continue;
            } else if (c != '1') {
                beep();
                continue;
            }

            if (fgetc(stdin) != ';' || fgetc(stdin) != '5') {
                beep();
                continue;
            }

            if ((c = fgetc(stdin)) == 'D') {
                on_prev_word();
            } else if (c == 'C') {
                on_next_word();
            } else {
                beep();
                continue;
            }
        } else if (c == 127) {
            on_erase();
        } else if (c <= 31) {
            beep();
        } else {
            u8 input[5] = {0};
            input[0] = c;

            u8 len = uc_char_len(c);
            if (len - 1 != 0) {
                fread(input + 1, len - 1, 1, stdin);
            }
            on_insert(input, len);
        }
    }

    clear();

    tcsetattr(fileno(stdin), 0, &term_old);
}