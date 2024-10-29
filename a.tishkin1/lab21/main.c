#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


int count;


void sigevent(int sig){
    if (sig == SIGQUIT) {
        printf("\nthe bell sounded %d times\n", count);

        exit(0);
    }

    if (sig == SIGINT) {
//        printf("\a\n");
        write(1,"\a",1);
        ++count;
    }
}

int main(int argc, char *argv[]){
    count = 0;

    sigset(SIGINT, sigevent);
    sigset(SIGQUIT, sigevent);

    while(1) {
        pause();
    }

    return 0;
}