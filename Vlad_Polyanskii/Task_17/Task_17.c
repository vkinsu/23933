#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define KILL 0x18
#define WERASE 0x17
#define ERASE 0x08
#define CLOSE_PROGRAMM 0x04

struct termios tty, savetty;

int main(){
    int fd = open("/dev/tty", O_RDWR);
    tcgetattr(fd, &tty);

    if (isatty(fileno(stdout)) == 0){
       fprintf(stderr,"stdout not terminal\n");
       exit(1);
    }
    savetty = tty;

    // Off echo and canonical mode
    tty.c_lflag &= ~(ECHO | ICANON); 

    // Set VMIN and VTIME
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;

    tcsetattr(fd, TCSANOW, &tty);

    int len = 0;
    char c;
    char buff[41] = {0};
    while(read(fd, &c, 1) == 1){
        if (iscntrl(c) || !isprint(c)) {
            switch (c){
                case ERASE:{
                    if(len > 0){
                        printf("\x1b[1D \x1b[1D");
                        len--;
                        buff[len] = '\0';
                    }
                    break;
                }
                case KILL:{
                    printf("\x1b[2K\r");
                    len = 0;
                    buff[0] = '\0';
                    break;
                }
                case WERASE:{
                    int begin = 0;
                    char prev = ' ';
                    for (int i = 0; i < len; i++) {
                        if (buff[i] != ' ' && prev == ' ') {
                            begin = i;
                        }
                        prev = buff[i];
                    }
                    buff[begin] = '\0';
                    printf("\x1b[%dD\x1b[K", len - begin);
                    len = begin; 
                    break;
                }
                case CLOSE_PROGRAMM:{
                    if (len == 0){
                        tcsetattr(fd, TCSAFLUSH, &savetty);
                        close(fd);
                        exit(0);
                    }
                    break;
                }
                default:{
                    printf("\a");
                    break;
                }
            }
        }
        else{
            if(len == 40){
                if(buff[len - 1] != ' '){
                    int begin = len - 1;
                    for(int i = len - 2; buff[i] != ' ' && i > -1; i--){
                        begin = i;
                    }

                    if(begin != 0){
                        printf("\x1b[%dG", begin);
                        printf("\x1b[K");
                        printf("\n");
                        printf("%s", &buff[begin]);
                        len -= begin;
                        strcpy(buff, &buff[begin]);
                        buff[len] = '\0';
                    }
                    else {
                        len = 0;
                        buff[len] = '\0';
                        printf("\n");
                    }
                }
            }
            else{
                buff[len++] = c;
                buff[len] = '\0';
                printf("%c", c);
            }
        }
        fflush(NULL);
    }
    tcsetattr(fd, TCSAFLUSH, &savetty);
   
    return 0;
}