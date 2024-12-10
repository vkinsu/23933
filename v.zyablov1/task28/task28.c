#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_sorted_random_numbers() {
    // Открываем пайп для передачи данных в команду sort
    FILE *pipe = popen("sort -n", "w");
    if (!pipe) {
        perror("popen failed");
        exit(1);
    }

    // srand(time(NULL));  // Инициализируем генератор случайных чисел

    // Генерируем 100 случайных чисел в диапазоне от 0 до 99 и передаем в pipe
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
          fprintf(pipe, "%d ", rand() % 100);  // Генерируем и передаем число
        }
        fprintf(pipe, "\n");
    }

    // Закрываем пайп, чтобы завершить передачу данных
    pclose(pipe);
}

int main() {
    generate_sorted_random_numbers();
    return 0;
}
