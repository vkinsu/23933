#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/31-server.sock"

int main(void) {
    unlink(SOCKET_PATH);

    // create socket
    int listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        perror("socket");
        return 1;
    }

    // make socket non blocking
    int flags = fcntl(listen_socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }

    if (fcntl(listen_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
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

    size_t fds_n = 1;
    size_t fds_cap = 2;
    struct pollfd *fds = malloc(sizeof(struct pollfd) * fds_cap);

    fds[0].fd = listen_socket;
    fds[0].events = POLLIN;

    while (1) {
        int pr = poll(fds, fds_n, -1);
        if (pr == -1) {
            perror("poll");
            return 1;
        }

        // process all fds
        for (size_t i = 0; i < fds_n; i++) {
            if (fds[i].revents == 0) {
                continue;
            }

            if (fds[i].revents != POLLIN) {
                // close(fds[i].fd);
                // fds[i].fd = 0;
                // continue;
                return 0;
            }

            // accept all new connections
            while (i == 0) {
                int conn = accept(listen_socket, NULL, NULL);
                if (conn == -1) {
                    if (errno == EWOULDBLOCK) {
                        break;
                    } else {
                        perror("accept");
                        return 1;
                    }
                }

                if (fds_n + 1 < fds_cap) {
                    fds_cap += fds_cap / 2;
                    fds = realloc(fds, fds_cap * sizeof(struct pollfd));
                }

                fds[fds_n].fd = conn;
                fds[fds_n].events = POLLIN;
                fds[fds_n].revents = 0;
                fds_n++;
            }

            // process new data
            while (i != 0) {
                char buf[512];

                ssize_t len = recv(fds[i].fd, buf, sizeof(buf), MSG_DONTWAIT);

                if (len == -1) {
                    if (errno == EWOULDBLOCK) {
                        break;
                    } else {
                        perror("accept");
                        return 1;
                    }
                }

                if (len == 0) {
                    fds[i].fd = 0;
                    return 0;
                }

                for (int i = 0; i < len; i++) {
                    buf[i] = toupper(buf[i]);
                }

                fwrite(buf, len, 1, stdout);
            }

            fds[i].revents = 0;
        }

        // compact
        size_t count = 0;
        size_t left = 0;
        size_t right = 0;
        while (right < fds_n) {
            if (fds[right].fd != 0) {
                fds[left] = fds[right];
                left++;
                count++;
            }
            right++;
        }
        fds_n = count;
    }

    close(listen_socket);
    unlink(SOCKET_PATH);
}