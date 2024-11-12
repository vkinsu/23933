#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <fcntl.h>


#define SOCKET_PATH "/tmp/unix_socket"
#define MAX_EVENTS 10

void handle_client(int client_sock) {
    char buffer[256];
    ssize_t bytes_read;

    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = (char)toupper(buffer[i]);
        }
        printf("Received: %s\n", buffer);
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock, epoll_fd;
    struct sockaddr_un addr;
    struct epoll_event event, events[MAX_EVENTS];

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);  // create Unix domain socket
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("failed to bind server sock");
        close(server_sock);
        return 1;
    }

    printf("Waiting for client...\n");
    listen(server_sock, 5);

    fcntl(server_sock, F_SETFL, O_NONBLOCK);

    epoll_fd = epoll_create1(0);

    event.events = EPOLLIN;
    event.data.fd = server_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event) == -1) {
        perror("epoll_ctl: server_sock");
        close(epoll_fd);
        close(server_sock);
        return 1;
    }

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_sock) {
                client_sock = accept(server_sock, NULL, NULL);
                if (client_sock == -1) {
                    perror("accept");
                    continue;
                }

                fcntl(client_sock, F_SETFL, O_NONBLOCK);

                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) == -1) {
                    perror("epoll_ctl: client_sock");
                    close(client_sock);
                    continue;
                }
            } else if (events[i].events & EPOLLIN) {
                client_sock = events[i].data.fd;
                handle_client(client_sock);

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
            }
        }
    }

    unlink(SOCKET_PATH);
    close(epoll_fd);
    close(server_sock);

    return 0;
}
