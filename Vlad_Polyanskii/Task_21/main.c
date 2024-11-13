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
struct termios tty, savetty;
int fd;

void sigcatch(int sig){
    if(sig == SIGINT){
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

    if (isatty(fd) == 0){
       fprintf(stderr,"Can't open tty\n");
       exit(1);
    }

    signal(SIGINT, sigcatch);
    signal(SIGQUIT, sigcatch);

    char buff;
    while(1){
        scanf("%c", &buff);
    }

    return 0;
}