#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void status_bits(struct stat path_info){
    if(S_ISDIR(path_info.st_mode)){
        putchar('d');
    }
    else if(S_ISREG(path_info.st_mode)){
        putchar('-');
    }
    else{
        putchar('?');
    }
}

void user_access(struct stat path_info){
    if((path_info.st_mode & S_IRUSR) != 0){
        putchar('r');
    }
    else putchar('-');

    if((path_info.st_mode & S_IWUSR) != 0){
        putchar('w');
    }
    else putchar('-');

    if((path_info.st_mode & S_IXUSR) != 0){
        putchar('x');
    }
    else putchar('-');
}

void group_access(struct stat path_info){
    if((path_info.st_mode & S_IRGRP) != 0){
        putchar('r');
    }
    else putchar('-');

    if((path_info.st_mode & S_IWGRP) != 0){
        putchar('w');
    }
    else putchar('-');

    if((path_info.st_mode & S_IXGRP) != 0){
        putchar('x');
    }
    else putchar('-');
}

void other_access(struct stat path_info){
    if((path_info.st_mode & S_IROTH) != 0){
        putchar('r');
    }
    else putchar('-');

    if((path_info.st_mode & S_IWOTH) != 0){
        putchar('w');
    }
    else putchar('-');

    if((path_info.st_mode & S_IXOTH) != 0){
        putchar('x');
    }
    else putchar('-');
}

void usr_grp_names(struct stat path_info){
    struct passwd *user_info;
    struct group *group_info;
    user_info = getpwuid(path_info.st_uid);
    group_info = getgrgid(path_info.st_gid);
    printf("%s %s", user_info->pw_name, group_info->gr_name);
}

void file_size(struct stat path_info){
    if(S_ISREG(path_info.st_mode)){
        printf("%ld", path_info.st_size);
    }
    else putchar('_');
}

void print_name(char* path){
    int len = strlen(path);
    char* name = path;
    for(int i = len - 1; i > -1; i--){
        if(path[i] == '/'){
            name = &path[i + 1];
            break;
        }
    }
    printf("%s", name);
}

void print_stat(char* path, struct stat path_info){
    if(stat(path, &path_info) == 0){
        status_bits(path_info);
        user_access(path_info);
        group_access(path_info);
        other_access(path_info);
        printf(" %ld ", path_info.st_nlink);
        usr_grp_names(path_info);
        putchar(' ');
        file_size(path_info);
        putchar(' ');
        char* time = ctime(&path_info.st_mtime);
        time[strlen(time) - 1] = '\0';
        printf("%s ", time);
        print_name(path);
        putchar('\n');
    }
    else{
        char buff[256];
        sprintf(buff, "ERROR Can't get stats from %s", path);
        perror(buff);
    }
}

int main(int argc, char** argv){
    struct stat path_info;
    for(int i = 0; i < argc; i++){
        print_stat(argv[i], path_info);
    }
    return 0;
}