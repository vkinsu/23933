#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;
volatile sig_atomic_t quit = 0;

void on_sigint(int signo) {
    count++;
    write(1, "\007\n", 2);
}

void on_sigquit(int signo) {
    quit = 1;
}

int main(void) {
    if (signal(SIGINT, on_sigint) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    if (signal(SIGQUIT, on_sigquit) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    while (!quit) {
        pause();
    }

    printf("\nbeeps: %d\n", count);
}