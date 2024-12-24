#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>

int main()
{
 struct tm* C_tm;
 time_t now = time(0);
 now -= 25200;
 C_tm = gmtime(&now);
 printf("Time in California: %d/%d/%d %d:%d:%d\n", C_tm->tm_mday, C_tm->tm_mon + 1, C_tm->tm_year + 1900, C_tm->tm_hour, C_tm->tm_min, C_tm->tm_sec);
 return 0;
}
