#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int beep_count = 0;

void signal_handler(int sig) {
  if (sig == SIGINT) {
    write(1, "\a", 1);
    beep_count++;
  } 
  else if (sig == SIGQUIT) {
    printf("\nSignal count: %d\n", beep_count);
    exit(0);
  }
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);

  while (1) {
    sleep(1);
  }

  return 0;
}
