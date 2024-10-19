#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int main() {
    // Переменные для хранения реального и эффективного UID
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    // Печать реального и эффективного идентификаторов пользователя
    printf("Реальный UID: %d, Эффективный UID: %d\n", real_uid, effective_uid);

    // Попытка открыть файл "file"
    FILE *file = fopen("file", "r");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
    } else {
        printf("Файл открыт успешно!\n");
        fclose(file);
    }

    // Сделать так, чтобы реальный и эффективный UID совпадали
    if (setuid(real_uid) == -1) {
        perror("Ошибка при установке идентификатора пользователя с помощью setuid");
        exit(EXIT_FAILURE);
    }

    // Повторная печать реального и эффективного идентификаторов пользователя после setuid
    printf("После setuid(): Реальный UID: %d, Эффективный UID: %d\n", getuid(), geteuid());

    // Повторная попытка открыть файл
    file = fopen("file", "r");
    if (file == NULL) {
        perror("Ошибка при повторном открытии файла");
    } else {
        printf("Файл повторно открыт успешно!\n");
        fclose(file);
    }

    return 0;
}

