#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    time_t now;
    struct tm *sp;
    setenv("TZ", "PST8PDT", 1);
    
    time(&now);

    printf("%s", ctime(&now));

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n",
            sp->tm_mon + 1, sp->tm_mday,
            1900 + sp->tm_year, sp->tm_hour,
            sp->tm_min, tzname[sp->tm_isdst]);
    return 0;
}