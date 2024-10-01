#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  if (argc < 1) {
    perror("No file name.\n");
    return 1;
  }

  FILE* output = fopen(argv[1], "w");

  if (output) {
    fprintf(output, "Real UID: %d\nEffective UID: %d\n", getuid(), geteuid());
    fclose(output);
  }
  else {
    perror("Error with output file.\n");
  }

  return 0;
}
