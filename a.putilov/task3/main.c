#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  if (argc != 2) {
	perror("Wrong arguments count.\n");
	return -1;
  }

  printf("Real UID: %d\nEffective UID: %d\n", getuid(), geteuid());

  FILE* file = fopen(argv[1], "r");
  if (file) {
	fclose(file);
  }
  else {
	perror(argv[1]);
  }

  setuid(geteuid());
  printf("Real UID: %d\nEffective UID: %d\n", getuid(), geteuid());

  file = fopen(argv[1], "r");
  if (file) {
	fclose(file);
  }
  else {
	perror(argv[1]);
	return -1;
  }

  return 0;
}
