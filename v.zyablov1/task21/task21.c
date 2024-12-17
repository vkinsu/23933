#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>  // Для write и pause

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
    sigint_count++;
    write(STDOUT_FILENO, "\a", 1);  // Звуковой сигнал
}

void handle_sigquit(int sig) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "\nSIGINT сигналов получено: %d\n", sigint_count);
    write(STDOUT_FILENO, buffer, len);
    _exit(0);  // Немедленный выход
}

int main() {
    struct sigaction sa_int, sa_quit;

    // Настройка обработчика SIGINT
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0;  // Отключаем дополнительные флаги
    sigemptyset(&sa_int.sa_mask);  // Не блокируем сигналы
    sigaction(SIGINT, &sa_int, NULL);

    // Настройка обработчика SIGQUIT
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = 0;
    sigemptyset(&sa_quit.sa_mask);
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Ctrl+C для звукового сигнала или Ctrl+\\ (Ctrl+4) для выхода.\n");

    while (1) {
        pause();  // Ожидание сигналов
    }

    return 0;
}

