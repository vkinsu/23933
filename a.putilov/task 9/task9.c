#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;

    // Проверка количества аргументов
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];  // Имя файла из аргументов командной строки

    // Создание дочернего процесса
    pid = fork();

    if (pid < 0) {
        perror("Error creating process");
        return 1;
    } else if (pid == 0) {
        // Дочерний процесс: выполняем команду cat
        printf("Child process: displaying contents of %s\n", filename);
        execlp("cat", "cat", filename, (char *)NULL);
        perror("Error executing cat");
        exit(1);  // Если execlp() не удался
    } else {
        // Родительский процесс
        printf("***\nParent process: this message appears before waiting for child.\n***\n");

        // Ожидание завершения дочернего процесса
        waitpid(pid, NULL, 0);

        printf("***\nParent process: child has finished.\n***\n");
    }

    return 0;
}

