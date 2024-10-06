#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    time_t now;
    struct tm* timeinfo;
    setenv("TZ", "America/Los_Angeles", 1);
    tzset();
    time(&now);
    timeinfo = localtime(&now);
    printf("%s", asctime(timeinfo));
    return 0;
}
