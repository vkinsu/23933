#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 2
#define SOCK_PATH "./unix_socket"
#define MAX_EVENTS 10

int main() {
    struct sockaddr_un server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    int server_s, client_s;

    server_s = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCK_PATH);

    if (bind(server_s, (struct sockaddr*)(&server_addr), sizeof(server_addr)) < 0) {
        perror("Server binding error!\n");
        return 1;
    }

    printf("Waiting for a client...\n");
    fflush(stdout);
    listen(server_s, MAX_CLIENTS);

    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1 failed");
        return 1;
    }

    struct epoll_event event, events[MAX_EVENTS];
    event.data.fd = server_s;
    event.events = EPOLLIN;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_s, &event) == -1) {
        perror("epoll_ctl failed");
        return 1;
    }


    socklen_t client_addr_len = sizeof(client_addr);
    int new_socket;
    char buf[1024];
    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            return 1;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_s) {
                if ((new_socket = accept(server_s, (struct sockaddr*)(&client_addr), &client_addr_len)) < 0) {
                    perror("Accept failed!\n");
                    continue;
                }
                printf("New client was connected.\n");
                event.data.fd = new_socket;
                event.events = EPOLLIN;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                    perror("epoll_ctl failed");
                    close(new_socket);
                    continue;
                }
            } else {
                ssize_t bcount = recv(events[i].data.fd, buf, 1024, 0);
                if (bcount == 0) {
                    printf("Client was disconnected.\n");
                    close(events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                } else if (bcount > 0) {
                    for (int j = 0; j < bcount; j++) {
                        if (97 <= (int)buf[j] <= 122) {
                            buf[j] = (char)toupper(buf[j]);
                        }
                    }
                    buf[bcount] = '\0';
                    printf("Upper recv: %s\n", buf);
                } else {
                    perror("Recv failed!\n");
                    close(events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
            }
        }
    }

    close(server_s);
    close(epfd);
    unlink(SOCK_PATH);

    return 0;
}

