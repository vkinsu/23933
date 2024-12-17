#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

void cleanup() {
    unlink(SOCKET_PATH);
}

int main() {
    int server_sock, client_sock, max_sd, activity;
    struct sockaddr_un addr;
    fd_set read_fds;

    signal(SIGINT, cleanup);

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for the client to connect...\n");
    
    int client_sockets[MAX_CLIENTS] = {0};

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_sd = server_sock;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
            }
            if (client_sockets[i] > max_sd) {
                max_sd = client_sockets[i];
            }
        }

        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("select");
        }

        if (FD_ISSET(server_sock, &read_fds)) {
            if ((client_sock = accept(server_sock, NULL, NULL)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("Client connected.\n");

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_sock;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &read_fds)) {
                char buf[BUFFER_SIZE];
                ssize_t num_read = read(client_sockets[i], buf, sizeof(buf) - 1);
                if (num_read > 0) {
                    buf[num_read] = '\0';
                    to_uppercase(buf);
                    printf("Message received from a client: %s\n", buf);

                    if (strcmp(buf, "EXIT") == 0) {
                        printf("Exit command received. Shutting down server...\n");
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_sockets[j] != 0) {
                                close(client_sockets[j]);
                            }
                        }
                        close(server_sock);
                        cleanup();
                        exit(EXIT_SUCCESS);
                    }
                } else {
                    if (num_read == 0) {
                        printf("The client disconnected.\n");
                    } else {
                        perror("read");
                    }
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                }
            }
        }
    }

    close(server_sock);
    cleanup();
    return 0;
}
