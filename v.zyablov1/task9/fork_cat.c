#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Создаем подпроцесс

    if (pid < 0) {
        perror("Ошибка при вызове fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Это дочерний процесс
        // Выполняем команду cat
        execlp("cat", "cat", "long_file.txt", (char *)NULL);
        // Если execlp возвращается, значит произошла ошибка
        perror("Ошибка при выполнении cat");
        exit(EXIT_FAILURE);
    } else {
        // Это родительский процесс
        printf("Родительский процесс: подпроцесс с PID %d запущен.\n", pid);

        // Ожидаем завершения дочернего процесса
        int status;
        waitpid(pid, &status, 0);

        // После завершения дочернего процесса
        printf("Родительский процесс: подпроцесс завершен.\n");
    }

    return EXIT_SUCCESS;
}

