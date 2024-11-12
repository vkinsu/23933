#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>

int counter = 0;
char bell = 7;
struct termios tty;
int fd;

void handler(int sig){
    if (sig == SIGINT){
        counter++;
        write(fd, &bell, sizeof(char));
    }
    else if (sig == SIGQUIT){
        printf("\n%d - num of bells\n", counter);
        exit(0);
    }
}

int main(){
    fd = open("/dev/tty", O_WRONLY);
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);

    char buff;
    while(1){
        scanf("%c", &buff);
    }

    return 0;
}