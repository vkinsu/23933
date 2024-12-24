#include "server.h"
#include "signals.h"
#include "worker.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define SOCKET_PATH "socket"
#define MAX_CLIENTS 2


int setup_server_socket() {
    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd == -1) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Bind error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (listen(fd, MAX_CLIENTS) == -1) {
        perror("Listen error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    return fd;
}

int server_run(int argc, char** argv, const char* client_path) {
    (void)argc;
    int server_fd = setup_server_socket();
    set_signal_handlers();

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork error");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (pid == 0) {  // Child process: launch client
        if (execvp(client_path, argv) == -1) {
            perror("execvp error");
            return EXIT_FAILURE;
        }
    } else {
        pthread_data pd[MAX_CLIENTS];
        pthread_t tids[MAX_CLIENTS];

        int connects = 0;
        while (connects < MAX_CLIENTS) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                continue;
            }

            pd[connects] = (pthread_data){.client_fd = client_fd, .client_id = connects, .child_pid = pid};

            if (pthread_create(&tids[connects], NULL, process_client, &pd[connects]) != 0) {
                perror("Thread creation error");
                close(server_fd);
                return EXIT_FAILURE;
            }
            connects++;
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            pthread_join(tids[i], NULL);
        }

        wait(NULL);
        close(server_fd);
    }
    return 0;
}

int main(int argc, char** argv) {
    return server_run(argc, argv, "./client");
}
