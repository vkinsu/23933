#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

int main() { time_t rawtime; struct tm * timeinfo;

setenv("TZ", "AMERICA/LOS_ANGELES", 1);
tzset();

time(&rawtime);
timeinfo = localtime(&rawtime);

printf("Текущее время в Калифорнии: %s", asctime(timeinfo));

return 0;
}
