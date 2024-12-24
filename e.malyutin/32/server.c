#include <aio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/31-server.sock"
#define BUFFER_SIZE 32

void free_aiocb(struct aiocb *cb) {
    free((void *)cb->aio_buf);
    free(cb);
}

void aio_notify_cb(union sigval sigval) {
    struct aiocb *cb = (struct aiocb *)sigval.sival_ptr;

    char *buf = (char *)cb->aio_buf;

    ssize_t n_bytes = aio_return(cb);

    if (n_bytes > 0) {
        int i = 0;
        for (i = 0; i < n_bytes; i++) {
            if (buf[i] >= 'a' && buf[i] <= 'z') {
                buf[i] = buf[i] - 'a' + 'A';
            }
        }
        buf[n_bytes] = '\0';

        printf("%s", buf);

        // clear
        memset(buf, 0, BUFFER_SIZE);

        // schedule next read
        aio_read(cb);
    } else if (n_bytes == 0) {
        printf("disconnect\n");
        close(cb->aio_fildes);
        free_aiocb(cb);
    } else {
        perror("can't read from client?");
        close(cb->aio_fildes);
    }
}

int main(void) {
    unlink(SOCKET_PATH);

    // create socket
    int listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        perror("socket");
        return 1;
    }

    // bind socket
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(listen_socket, 128) == -1) {
        perror("listen");
        return 1;
    }

    while (1) {
        // accept connection
        int conn = accept(listen_socket, NULL, NULL);
        if (conn == -1) {
            perror("accept");
            return 1;
        }

        struct aiocb *cb = malloc(sizeof(struct aiocb));
        if (cb == NULL) {
            perror("malloc");
            return 0;
        }

        cb->aio_buf = malloc(BUFFER_SIZE);
        if (cb->aio_buf == NULL) {
            perror("malloc");
            return 0;
        }

        cb->aio_nbytes = BUFFER_SIZE;
        cb->aio_fildes = conn;
        cb->aio_lio_opcode = LIO_READ;
        cb->aio_offset = 0;
        cb->aio_sigevent.sigev_notify = SIGEV_THREAD;
        cb->aio_sigevent.sigev_notify_function = aio_notify_cb;
        cb->aio_sigevent.sigev_notify_attributes = NULL;
        cb->aio_sigevent.sigev_value.sival_ptr = cb;

        if (aio_read(cb) == -1) {
            perror("can't aio_read");
            free_aiocb(cb);
        }
    }
}
