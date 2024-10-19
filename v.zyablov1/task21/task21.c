#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int signal_count = 0;
void sigint_handler(int signo) {
    if (signo == SIGINT) {
        printf("\a");
        signal_count++;
    }
}


void sigquit_handler(int signo) {
    if (signo == SIGQUIT) {
        printf("\nTotal signals: %d\n", signal_count);
        exit(0);
    }
}


int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error setting up SIGINT handler");
        exit(1);
    }

    if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) {
        perror("Error setting up SIGQUIT handler");
        exit(2);
    }

    while (1) {
        pause();
    }
}
