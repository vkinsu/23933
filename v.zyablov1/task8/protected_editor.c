#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

void change_permissions(const char *filename, mode_t mode) {
    if (chmod(filename, mode) < 0) {
        perror("Ошибка при изменении прав доступа");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    // Захватываем файл, устанавливая права на чтение для группы
    change_permissions(filename, S_IRUSR | S_IRGRP | S_IWUSR);

    // Запускаем текстовый редактор
    printf("Запуск редактора. Вы можете редактировать файл '%s'.\n", filename);
    system("nano"); // Замените "nano" на ваш любимый редактор

    // Восстанавливаем права доступа (например, устанавливаем права на запись для группы)
    change_permissions(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    printf("Редактор завершен. Права доступа восстановлены.\n");
    return EXIT_SUCCESS;
}

