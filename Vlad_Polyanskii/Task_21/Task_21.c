#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>

int counter = 0;
int fd;

void sigcatch(int sig){
    signal(sig, SIG_IGN);
    if(sig == SIGINT){
        counter++;
        write(fd, "\a", 1);
    }
    else if (sig == SIGQUIT){
        char message[64];
        sprintf(message, "\n%d - num of bells\n", counter);
        write(fd, message, strlen(message));
        exit(0);
    }
    signal(sig, sigcatch);
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