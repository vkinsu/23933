#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

int main() {

	struct tm* my_tm;
	time_t now = time(0);
	now -= 25200;
	my_tm = gmtime(&now);

	printf("California> %d/%d/%d %d:%d:%d\n", my_tm->tm_mday, my_tm->tm_mon + 1, my_tm->tm_year + 1900,
								my_tm->tm_hour, my_tm->tm_min, my_tm->tm_sec);

	return 0;
}
