#include "client.h"
#include "connection.h"
#include "signals.h"
#include "message_handler.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_PATH "socket"
#define CLIENT_COUNT 2

int fds[CLIENT_COUNT];

int client_run() {
    memset((void*)servers_done, 0, sizeof(servers_done));
    init_sockets(fds, CLIENT_COUNT);
    set_signal_handlers();

    struct sockaddr_un addr = create_socket_address(SOCKET_PATH);

    if (make_connect(fds[1], &addr)) {
        perror("Connect error");
        return 1;
    }

    if (make_connect(fds[0], &addr)) {
        perror("Connect error");
        return 1;
    }

    pthread_t tids[CLIENT_COUNT];
    pthread_data pd[CLIENT_COUNT];
    pid_t ppid = getppid();

    pd[0] = create_pthread_data(fds[0], 0, "Hello world!", ppid);
    pd[1] = create_pthread_data(fds[1], 1, "Another message", ppid);

    for (int i = 0; i < CLIENT_COUNT; ++i) {
        if (pthread_create(&tids[i], NULL, send_message, &pd[i]) != 0) {
            perror("Thread create error");
            return 1;
        }
    }

    for (int i = 0; i < CLIENT_COUNT; ++i) {
        pthread_join(tids[i], NULL);
    }

    close_fds(fds, CLIENT_COUNT);
    return 0;
}

int main() {
    return client_run();
}
