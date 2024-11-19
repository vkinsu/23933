#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *filename) {
    struct stat file_stat;

    if (lstat(filename, &file_stat) < 0) {
        perror("lstat");
        return;
    }

    char type = '?';
    if (S_ISDIR(file_stat.st_mode))
        type = 'd';
    else if (S_ISREG(file_stat.st_mode))
        type = '-';

  
    char permissions[10];
    permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';


    int links = file_stat.st_nlink;

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group  *gr = getgrgid(file_stat.st_gid);

    off_t size = S_ISREG(file_stat.st_mode) ? file_stat.st_size : 0;

    char time_string[100];
    struct tm lt;
    localtime_r(&file_stat.st_mtime, &lt);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M", &lt);
    
    printf("%c%s %2d %s %s %*lld %s %s\n",
           type, permissions, links,
           pw ? pw->pw_name : "UNKNOWN",
           gr ? gr->gr_name : "UNKNOWN",
           (type == '-' ? 10 : 0),
           (type == '-' ? size : 0),
           time_string,
           strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename
    );
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("error arguments");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}
