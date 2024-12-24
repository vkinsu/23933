#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <pthread.h>

typedef struct pthread_data {
    int fd;
    int id;
    char* message;
    int len;
    pid_t ppid;
} pthread_data;

pthread_data create_pthread_data(int fd, int id, char* message, pid_t ppid);
void* send_message(void* arg);

#endif

