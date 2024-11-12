#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_permissions(mode_t mode) {
    printf("%c", (S_ISDIR(mode)) ? 'd' : (S_ISREG(mode) ? '-' : '?'));
        printf("%c", (mode & S_IRUSR) ? 'r' : '-');  // Read permission for owner
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');  // Write permission for owner
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');  // Execute permission for owner

    printf("%c", (mode & S_IRGRP) ? 'r' : '-');  // Read permission for group
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');  // Write permission for group
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');  // Execute permission for group

    printf("%c", (mode & S_IROTH) ? 'r' : '-');  // Read permission for others
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');  // Write permission for others
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');  // Execute permission for others
}

void print_file_info(const char *filename) {
    struct stat statbuf;
    
    if (stat(filename, &statbuf) == -1) {
        perror("stat");
        return;
    }
    
    struct passwd *pwd = getpwuid(statbuf.st_uid);
    struct group *grp = getgrgid(statbuf.st_gid);

    print_permissions(statbuf.st_mode);
    
    printf(" %-3ld", statbuf.st_nlink);
    
    printf(" %-8s %-8s", pwd ? pwd->pw_name : "unknown", grp ? grp->gr_name : "unknown");
    
    if (S_ISREG(statbuf.st_mode)) {
        printf(" %-8ld", statbuf.st_size);
    } else {
        printf("         ");
    }

    char timebuf[64];
    struct tm *tm_info = localtime(&statbuf.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %-16s", timebuf);

    const char *base_filename = strrchr(filename, '/');
    printf(" %s\n", base_filename ? base_filename + 1 : filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}
