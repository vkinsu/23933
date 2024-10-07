#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

extern char* tzname[];

int main() {
    time_t now;
    struct tm* sp;

    putenv("TZ=America/Los_Angeles");

    now = time(NULL);
    printf("%s", ctime(&now));

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year - 100, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);

    return 0;
}
