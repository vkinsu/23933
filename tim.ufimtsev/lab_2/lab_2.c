#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern char *tzname[]; // имена часовых поясов

int main() {
    time_t now;
    struct tm *sp;
    char *old_tz;

    old_tz = getenv("TZ");

    setenv("TZ", "PST8PDT", 1);
    tzset();

    (void) time(&now);

    sp = localtime(&now);
    printf("Current PST time: %d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);

    if (old_tz) {
        setenv("TZ", old_tz, 1);
    } else {
        unsetenv("TZ");
    }
    tzset();

    exit(0);
}
