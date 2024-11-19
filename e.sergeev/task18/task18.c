#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

// Функция для получения типа файла
char get_file_type(mode_t mode) {
    if (S_ISDIR(mode)) return 'd';
    else if (S_ISREG(mode)) return '-';
    else return '?';
}

// Функция для получения прав доступа в виде строки
void get_permissions(mode_t mode, char *perms) {
    perms[0] = (mode & S_IRUSR) ? 'r' : '-';
    perms[1] = (mode & S_IWUSR) ? 'w' : '-';
    perms[2] = (mode & S_IXUSR) ? 'x' : '-';
    perms[3] = (mode & S_IRGRP) ? 'r' : '-';
    perms[4] = (mode & S_IWGRP) ? 'w' : '-';
    perms[5] = (mode & S_IXGRP) ? 'x' : '-';
    perms[6] = (mode & S_IROTH) ? 'r' : '-';
    perms[7] = (mode & S_IWOTH) ? 'w' : '-';
    perms[8] = (mode & S_IXOTH) ? 'x' : '-';
    perms[9] = '\0';
}

// Основная функция для обработки одного файла
void list_file_info(const char *filepath) {
    struct stat file_stat;
    if (stat(filepath, &file_stat) == -1) {
        perror("Error getting file stats");
        return;
    }

    // Тип файла и права доступа
    char file_type = get_file_type(file_stat.st_mode);
    char permissions[10];
    get_permissions(file_stat.st_mode, permissions);

    // Количество жёстких ссылок
    int links = file_stat.st_nlink;

    // Имя владельца и группы
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    char *owner = pw ? pw->pw_name : "unknown";
    char *group = gr ? gr->gr_name : "unknown";

    // Размер файла (только для обычных файлов)
    off_t size = (file_type == '-') ? file_stat.st_size : 0;

    // Время последней модификации
    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%b %d %H:%M", localtime(&file_stat.st_mtime));

    // Имя файла (без пути)
    char *filename = basename((char *)filepath);

    // Вывод информации о файле
    printf("%c%s %3d %-8s %-8s %6ld %s %s\n",
           file_type, permissions, links, owner, group,
           (file_type == '-') ? size : 0, mod_time, filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ... fileN]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        list_file_info(argv[i]);
    }

    return 0;
}

