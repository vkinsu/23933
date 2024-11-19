#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
    sigint_count++;
    printf("\a");
    fflush(stdout);
}

void handle_sigquit(int sig) {
    printf("\nSIGINT received %d times\n", sigint_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    while (1) {
        pause();
    }

    return 0;
}
