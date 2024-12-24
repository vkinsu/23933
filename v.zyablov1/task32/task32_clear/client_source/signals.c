#include "signals.h"
#include <signal.h>

volatile int servers_done[2];

void handler(int sig) {
    if (sig == SIGRTMIN + 2) {
        servers_done[0] = 1;
    }
    if (sig == SIGRTMIN + 5) {
        servers_done[1] = 1;
    }
}

void set_signal_handlers() {
    for (int id = 0; id < 2; id++) {
        int sig = SIGRTMIN + id * 3;
        signal(sig + 2, handler);
    }
}

