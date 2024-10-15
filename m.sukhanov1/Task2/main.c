#include <stdio.h>  
#include <time.h>          // time() , struct tm, time_t, ctime(), localtime(), tzname[]
#include <stdlib.h>

int main(void) {

    extern char **environ;

    time_t now;
    struct tm *sp;

    (void) time(&now);

    printf("Date and time in Novosibirsk: %s\n", ctime(&now));

    putenv("TZ=PST8PDT");

    sp = localtime(&now);

    printf("Date and time in Los Angeles: %d/%d/%04d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
    
}

