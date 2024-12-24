#include "signals.h"
#include <signal.h>
#include <string.h>

volatile int end_messages[2] = {0};
volatile int beg_messages[2] = {0};

void begin_handler(int sig) {
    if (sig == SIGRTMIN) {
        beg_messages[0] = 1;
    }
    if (sig == SIGRTMIN + 3) {
        beg_messages[1] = 1;
    }
}

void end_handler(int sig) {
    if (sig == SIGRTMIN + 1) {
        end_messages[0] = 1;
    }
    if (sig == SIGRTMIN + 4) {
        end_messages[1] = 1;
    }
}

void set_signal_handlers() {
    for (int id = 0; id < 2; ++id) {
        int sig = SIGRTMIN + id * 3;
        signal(sig, begin_handler);
        signal(sig + 1, end_handler);
    }
}

