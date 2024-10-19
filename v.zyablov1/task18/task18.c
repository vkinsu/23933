#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"


//#define BUF_SIZE 1024
int buf_len = 1024;
char l = 0, //Vknown as a long format that displays detailed information about files and directories.
a = 0,      //VRepresent all files Include hidden files and directories in the listing.
t = 0,      //VSort files and directories by their last modification time, displaying the most recently modified ones first.
r = 0,      //Vknown as reverse order which is used to reverse the default order of listing.
S = 0,      //VSort files and directories by their sizes, listing the largest ones first.
R = 0,      //VList files and directories recursively, including subdirectories.
i = 0,      //Vknown as inode which displays the index number (inode) of each file and directory.
g = 0,      //Vknown as group which displays the group ownership of files and directories instead of the owner.
h = 0,      //VPrint file sizes in human-readable format (e.g., 1K, 234M, 2G).
d = 0;      //VList directories themselves, rather than their contents.


int dirSort(const struct dirent **a, const struct dirent **b) {
    char path_a[PATH_MAX];
    char path_b[PATH_MAX];
    snprintf(path_a, PATH_MAX, "%s/%s", ".", (*a)->d_name);
    snprintf(path_b, PATH_MAX, "%s/%s", ".", (*b)->d_name);

    struct stat a_buf;
    struct stat b_buf;
    int ab = stat(path_a, &a_buf);
    int bb = stat(path_b, &b_buf);

//    if (ab != 0) {
//        perror("stat for a failed");
//        return errno;
//    }
//
//    if (bb != 0) {
//        perror("stat for b failed");
//        return errno;
//    }

    int res;
    if(S) {
        res = b_buf.st_size - a_buf.st_size;
        if (!res)
            return strcmp((*a)->d_name, (*b)->d_name);
    }
    else if(t) {
        res = b_buf.st_mtim.tv_sec - a_buf.st_mtim.tv_sec;
        if (!res)
            return strcmp((*a)->d_name, (*b)->d_name);
    }
    else
        res = strcmp((*a)->d_name, (*b)->d_name);
    if(r)
        return res * -1;
    return res;
}


void printFileInfo(const char *filename, const char *path) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, PATH_MAX, "%s/%s", path, filename);

    struct stat fileStat;
    int f_size = stat(fullpath, &fileStat);

    if (f_size == -1) {
        perror("cant open file for stat\n");
        return;
    }

//    printf("path:%s, size:%ld\n", path, fileStat.st_size);

    if (i)
        printf("%llu ", (unsigned long long)fileStat.st_ino);

    if (l) {
        char *mod_time = ctime(&fileStat.st_mtime);
        mod_time[strlen(mod_time) - 1] = '\0';

        struct passwd *pwd = getpwuid(fileStat.st_uid);
        struct group *grp = getgrgid(fileStat.st_gid);

        printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
        printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

        printf(" %lu", (unsigned long)fileStat.st_nlink);

        if (!g) {
            printf(" %s", pwd ? pwd->pw_name : "unknown");
        }

        if (g) {
            printf(" %s", grp ? grp->gr_name : "unknown");
        }

        if (h) {
            char size_unit;
            double size_value;

            if (fileStat.st_size >= 1e9) {
                size_unit = 'G';
                size_value = fileStat.st_size / 1e9;
            } else if (fileStat.st_size >= 1e6) {
                size_unit = 'M';
                size_value = fileStat.st_size / 1e6;
            } else if (fileStat.st_size >= 1e3) {
                size_unit = 'K';
                size_value = fileStat.st_size / 1e3;
            } else {
                size_unit = 'B';
                size_value = fileStat.st_size;
            }

            printf(" %.1f%c ", size_value, size_unit);
        } else {
            printf(" %ld ", (long)fileStat.st_size);
        }

        printf("%s ", mod_time);
    }
    if(l)
        printf("\t%s\n", filename);
    else
        printf("%s  ", filename);
}


void listDir(const char *path){
    struct dirent **entries;
    int count;
    count = scandir(path, &entries, NULL, dirSort);
    if(count == 1){
        perror("scandir return error code");
        printf("%d\n", errno);
        return errno;
    }
    if(count <= 2)
        return;;
    printf(GREEN);
    printf(path);
    printf(RESET);
    printf("\n");
    char **subpaths = (char **)malloc(count * sizeof(char *));
    for (int i = 0; i < count; i++) {
        if (!a && entries[i]->d_name[0] == '.')
            continue;

        printFileInfo(entries[i]->d_name, path);

        subpaths[i] = NULL;
        if (R && entries[i]->d_type == DT_DIR && strlen(entries[i]->d_name) > 0 && strcmp(entries[i]->d_name, ".") != 0 && strcmp(entries[i]->d_name, "..") != 0) {
            char subpath[PATH_MAX];
            snprintf(subpath, PATH_MAX, "%s/%s", path, entries[i]->d_name);
            subpaths[i] = strdup(subpath);
//            printf("new subpath:%s:\n", subpaths[i]);
        }
    }

    printf("\n");
    for (int i = 0; i < count; i++)
        if (subpaths[i] != NULL && strlen(subpaths[i]) > 2)
            listDir(subpaths[i]);

//    for (int i = 0; i < count; i++) {
//        free(entries[i]);
//        free(subpaths[i]);
//    }
//    free(entries);
//    free(subpaths);
}

int main (int argc, char *argv[]){
//    takecwd:
//    char *cwd = (char*) calloc(buf_len, 1);
//    if(!getcwd(cwd, buf_len)){
//        if(errno == ERANGE){
//            perror("size is too less to write cwd, increasing and trying again\n");
//            buf_len *= 2;
//            free(cwd);
//            goto takecwd;
//        } else{
//            perror("getcwd");
//            return -1;
//        }
//    }
//    printf("cwd is:%s\n", cwd);

    char opt;
    while((opt = getopt(argc, argv, "latrSRighd")) != -1){
//        printf("flag is:%c:\n", opt);
        switch (opt) {
            case 'l':
                l = 1;
                break;
            case 'a':
                a = 1;
                break;
            case 't':
                t = 1;
                break;
            case 'r':
                r = 1;
                break;
            case 'S':
                S = 1;
                break;
            case 'R':
                R = 1;
                break;
            case 'i':
                i = 1;
                break;
            case 'g':
                g = 1;
                break;
            case 'h':
                h = 1;
                break;
            case 'd':
                d = 1;
                break;
        }
    }
    listDir(".");
    return 0;
}
