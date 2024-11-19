#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

char get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    return '?';
}

void get_permissions(mode_t mode, char *permissions) {
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
}

const char* get_file_name(const char *path) {
    const char *name = strrchr(path, '/');
    return name ? name + 1 : path;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_paths>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        struct stat statbuf;
        if (stat(argv[i], &statbuf) == -1) {
            perror("stat");
            continue;
        }

        char file_type = get_file_type(statbuf.st_mode);
        char permissions[10];
        get_permissions(statbuf.st_mode, permissions);
        nlink_t nlinks = statbuf.st_nlink;
        struct passwd *pw = getpwuid(statbuf.st_uid);
        struct group *gr = getgrgid(statbuf.st_gid);
        off_t size = statbuf.st_size;
        char mod_time[20];
        strftime(mod_time, sizeof(mod_time), "%b %d %H:%M", localtime(&statbuf.st_mtime));
        const char *file_name = get_file_name(argv[i]);

        printf("%c%s %lu %s %s %10ld %s %s\n",
               file_type, permissions, nlinks,
               pw ? pw->pw_name : "UNKNOWN",
               gr ? gr->gr_name : "UNKNOWN",
               file_type == '-' ? size : 0,
               mod_time, file_name);
    }

    return 0;
}
