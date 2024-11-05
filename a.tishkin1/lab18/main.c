#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>


void printFileInfo(const char *path) {
    struct stat fileStat;
    if (stat(path, &fileStat) == -1) {
        perror("failed to get stats\n");
        return;
    }

    // filetype
    char type;
    if (S_ISDIR(fileStat.st_mode)) {
        type = 'd';
    } else if (S_ISREG(fileStat.st_mode)) {
        type = '-';
    } else {
        type = '?';
    }

    // permissions
    char permissions[10];
    permissions[0] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';

    // uid and gid
    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group  *gr = getgrgid(fileStat.st_gid);

    int nlink = fileStat.st_nlink;
    off_t size = fileStat.st_size;

    // date modified
    char timebuf[100];
    struct tm *tm_info = localtime(&fileStat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);

    // filename
    const char *filename = strrchr(path, '/');
    if (filename == NULL) {filename = path;} else {++filename;}

    printf(
            "%-1c%-9s@ %-1d %-17s %-6s %lld %s %s\n",
            type, permissions, nlink, pw->pw_name, gr->gr_name, size, timebuf, filename
    );
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printFileInfo(".");
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        printFileInfo(argv[i]);
    }

    return 0;
}
