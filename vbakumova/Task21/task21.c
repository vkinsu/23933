#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t beep_count = 0; // Переменная для подсчёта сигналов SIGINT

// Обработчик для SIGINT
void handle_sigint(int sig) {
    (void)sig; // Игнорируем аргумент
    write(STDOUT_FILENO, "\a", 1); // Издаём звуковой сигнал
    beep_count++;
}

// Обработчик для SIGQUIT
void handle_sigquit(int sig) {
    (void)sig; // Игнорируем аргумент
    printf("\nПрограмма завершена. Количество сигналов SIGINT: %d\n", beep_count);
    exit(0);
}

int main() {
    struct sigaction sa_int, sa_quit;

    // Настройка обработчика SIGINT
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = SA_RESTART; // Автоматически перезапускать системные вызовы
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    // Настройка обработчика SIGQUIT
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = SA_RESTART;
    sigemptyset(&sa_quit.sa_mask);
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Программа запущена. Нажмите CTRL-C для звука и CTRL-\\ для выхода.\n");

    while (1) {
        pause(); // Ожидаем сигналов
    }

    return 0;
}
