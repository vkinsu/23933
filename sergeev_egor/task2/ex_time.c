#define _CRT_SECURE_NO_WARNINGS
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    time_t now = time(0);
    now -= 25200;
    struct tm* sp;
    sp = gmtime(&now);
    printf("Day: %d\nMonth: %d\nYear: %d\n%d:%d:%d", sp->tm_mday, sp->tm_mon + 1, 1900 + sp->tm_year,
        sp->tm_hour, sp->tm_min, sp->tm_sec);
    return 0;
}