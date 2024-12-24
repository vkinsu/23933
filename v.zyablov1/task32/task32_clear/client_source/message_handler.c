#include "message_handler.h"
#include "signals.h"

#include <aio.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

pthread_data create_pthread_data(int fd, int id, char* message, pid_t ppid) {
    pthread_data pd;
    pd.fd = fd;
    pd.id = id;
    pd.message = message;
    pd.len = strlen(message);
    pd.ppid = ppid;
    return pd;
}

void* send_message(void* arg) {
    pthread_data* pd = (pthread_data*)arg;
    int sig = SIGRTMIN + pd->id * 3;

    struct aiocb writerq;
    memset(&writerq, 0, sizeof(writerq));
    writerq.aio_fildes = pd->fd;
    writerq.aio_buf = pd->message;
    writerq.aio_nbytes = pd->len;

    const struct aiocb* requests[2] = {&writerq, NULL};

    aio_write(&writerq);
    kill(pd->ppid, sig);

    while (1) {
        aio_suspend(requests, 1, NULL);
        int ret = aio_return(&writerq);
        if (ret == -1 && errno != EINPROGRESS) {
            perror("aio write error");
            return arg;
        }
        if (ret != -1) break;
    }

    kill(pd->ppid, sig);
    printf("Client %d send full message\n", pd->id);

    while (servers_done[pd->id] != 1);
    close(pd->fd);
    printf("Client %d disconnect\n", pd->id);
    return NULL;
}

