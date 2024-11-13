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

void sigint_handler(int sig){
    counter++;
    write(fd, &bell, sizeof(char));
}

void sigquit_handler(int sig){
    printf("\n%d - num of bells\n", counter);
    exit(0);
}

int main(){
    fd = open("/dev/tty", O_WRONLY);
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);

    char buff;
    while(1){
        scanf("%c", &buff);
    }

    return 0;
}