#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>

typedef struct pthread_data {
    int client_fd;
    int client_id;
    pid_t child_pid;
} pthread_data;

void* process_client(void* arg);

#endif

