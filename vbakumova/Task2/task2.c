#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

int main() {
	time_t now;
	struct tm* sp;

	(void)time(&now);

	now -= 25200;
	sp = gmtime(&now);
	printf("California:  %d/%d/%d  %d:%d:%d\n", sp->tm_mday, sp->tm_mon + 1, sp->tm_year + 1900, sp->tm_hour, sp->tm_min, sp->tm_sec);
	exit(0);
}