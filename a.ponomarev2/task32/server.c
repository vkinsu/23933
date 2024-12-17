#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <ctype.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

int main() {
    int server_sock, client_sock, epoll_fd, event_count, i;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &ev) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct epoll_event events[MAX_EVENTS];
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count < 0) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_sock) {
                client_len = sizeof(client_addr);
                client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
                if (client_sock < 0) {
                    perror("accept");
                    continue;
                }

                printf("Client connected.\n");

                ev.events = EPOLLIN;
                ev.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &ev) < 0) {
                    perror("epoll_ctl");
                    exit(EXIT_FAILURE);
                }

            } else {
                ssize_t received = recv(events[i].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (received < 0) {
                    perror("recv");
                    close(events[i].data.fd);
                } else if (received == 0) {
                    printf("Client disconnected.\n");
                    close(events[i].data.fd);
                } else {
                    buffer[received] = '\0';
                    printf("Received from client: %s\n", buffer);

                    if (strcmp(buffer, "exit") == 0) {
                        printf("Exit command received. Shutting down server...\n");
                        close(events[i].data.fd); 
                        
                        close(server_sock);
                        unlink(SOCKET_PATH);
                        exit(EXIT_SUCCESS);
                    }

                    for (int j = 0; j < received; j++) {
                        buffer[j] = toupper((unsigned char)buffer[j]);
                    }
                    printf("Transformed: %s\n", buffer);
                }
            }
        }
    }

    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
