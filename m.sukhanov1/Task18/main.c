#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *path) {
    struct stat file_stat;

    if (lstat(path, &file_stat) == -1) { // 
        perror("Error accessing file");
        return;
    }

    // File type
    char file_type;
    if (S_ISREG(file_stat.st_mode)) {
        file_type = '-';
    } else if (S_ISDIR(file_stat.st_mode)) {
        file_type = 'd';
    } else {
        file_type = '?';
    }

    // Permissions
    char permissions[10] = {0};
    permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';

    // Number of links
    int links = file_stat.st_nlink;

    // Owner and group
    struct passwd *owner = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);

    char *owner_name = owner ? owner->pw_name : "unknown";
    char *group_name = grp ? grp->gr_name : "unknown";

    // File size (only for regular files)
    char size_str[20] = "";
    if (file_type == '-') {
        snprintf(size_str, sizeof(size_str), "%ld", file_stat.st_size);
    }

    
    char time_str[20];
    struct tm *mod_time = localtime(&file_stat.st_mtime);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", mod_time);

    
    const char *file_name = strrchr(path, '/'); // index of last '/'
    file_name = file_name ? file_name + 1 : path;

    
    printf("%c%s %3d %-8s %-8s %8s %s %s\n",
           file_type, permissions, links, owner_name, group_name, size_str, time_str, file_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    printf("%-1s%-9s %-3s %-8s %-8s %-8s %-14s  %-s\n",
           "T", "Perms", "Ln", "Owner", "Group", "Size", "Modified", "Name");

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

