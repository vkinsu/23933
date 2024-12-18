#include "worker.h"
#include "server.h"

#include "signals.h"
#include <aio.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void* process_client(void* arg) {
    pthread_data* pd = (pthread_data*)arg;

    struct aiocb readrq;
    memset(&readrq, 0, sizeof(readrq));
    readrq.aio_fildes = pd->client_fd;
    char sym = 0;
    readrq.aio_buf = &sym;
    readrq.aio_nbytes = 1;

    while (!beg_messages[pd->client_id]);

    int client_sig = SIGRTMIN + pd->client_id * 3;
    printf("Server start reading a message %d\n", pd->client_id);
    aio_read(&readrq);

    while (1) {
        int ret = aio_error(&readrq);
        if (ret == 0) {  // Success
            putchar(toupper(sym));
            aio_read(&readrq);
        } else if (ret == EAGAIN && end_messages[pd->client_id]) {
            printf("\nServer got a message %d\n", pd->client_id);
            close(pd->client_fd);
            kill(pd->child_pid, client_sig + 2);
            return NULL;
        } else if (ret != EINPROGRESS) {
            perror("aio_read error");
            close(pd->client_fd);
            kill(pd->child_pid, client_sig + 2);
            return arg;
        }
    }
}

