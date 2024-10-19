#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main() {
    //create termios struct (it keep all adjustments for using terminal)
    struct termios cur_termios;
    char c;
    tcgetattr(STDIN_FILENO, &cur_termios);
    //make new settings (ECHO = show symbols, that appear in console), (ICANON ~= EOF after any symbol)
    // new_termios.c_lflag &= ~(ICANON | ECHO);
    cur_termios.c_lflag &= ~(ICANON);
    //set new settings (TCSANOW = set these settings right now)
    tcsetattr(STDIN_FILENO, TCSANOW, &cur_termios);

    printf("Press a key (without enter)");
    c = getchar();

    printf("\nYou pressed '%c'\n", c);
    //make origin settings
    cur_termios.c_lflag |= (ICANON);
    //set origin settins
    tcsetattr(STDIN_FILENO, TCSANOW, &cur_termios);
    return 0;
}