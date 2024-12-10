#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

char *get_permissions(mode_t mode) {
    static char permissions[11];
    permissions[0] = (mode & S_IFDIR) ? 'd' : ((mode & S_IFREG) ? '-' : '?');
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        struct stat sb;
        if (stat(argv[i], &sb) == -1) {
            perror(argv[i]);
            continue;
        }

        struct passwd *pw = getpwuid(sb.st_uid);
        struct group *gr = getgrgid(sb.st_gid);
        char *username = (pw != NULL) ? pw->pw_name : "?";
        char *groupname = (gr != NULL) ? gr->gr_name : "?";
        char *filename = strrchr(argv[i], '/');
        filename = (filename != NULL) ? filename + 1 : argv[i];


        char size_str[20];
        if (S_ISREG(sb.st_mode)) {
            sprintf(size_str, "%lld", (long long)sb.st_size);
        } else {
            strcpy(size_str, "");
        }

        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", localtime(&sb.st_mtime));

        printf("%s %3lld %s %s %8s %15s %s\n", 
               get_permissions(sb.st_mode), (long long)sb.st_nlink, username, groupname, size_str, time_str, filename);
    }

    return 0;
}

