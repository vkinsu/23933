#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define BEL "\07"

int beep_count = 0;

// Обработчик для SIGINT
void handler_sigint(int signum) {
        write(STDOUT_FILENO, BEL, 1);
        beep_count++;
        signal(SIGINT, handler_sigint);
}
// Обработчик для SIGQUIT
void handler_sigquit(int signum) {
        printf("\nSignal SIGQUIT received. Beeped %d times. Exiting.\n", beep_count);
        exit(0);
}

int main() {
    // Устанавливаем обработчики сигналов
    signal(SIGINT, handler_sigint);
        signal(SIGQUIT, handler_sigquit);

    printf("Press CTRL-C for a beep. Press CTRL-\\ to quit.\n");
    
    while (1) {
        pause(); // Ожидание сигнала
    }
    
    return 0;
}


