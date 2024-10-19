#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <poll.h>
#include <sys/ioctl.h>


#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define BUF_SIZE 1024
#define MAX_CLIENTS 5
#define TIMEOUT 100000
#define POLL_SIZE 32
//it should be /tmp/my_sockets/ but die to have no access to this directory on studying server this path is local
#define sockets_path "."


int main() {
    struct pollfd poll_set[POLL_SIZE];
    int server_fd, client_fds[MAX_CLIENTS] = {0};
    fd_set read_fds, active_fds;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket.\n");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    char *cur_socket_path[BUF_SIZE];
//    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, getpid());
    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, 0);
    remove(cur_socket_path);
    strncpy(server_addr.sun_path, cur_socket_path, sizeof(server_addr.sun_path) - 1);

    if ((bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1) {
        perror("Bind.\n");
        close(server_fd);
        exit(2);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror(("Listen.\n"));
        close(server_fd);
        exit(3);
    }

    printf(GREEN "Server is listening on %s\n" RESET, cur_socket_path);

    memset(poll_set, 0, sizeof(poll_set));
    poll_set[0].fd = server_fd;
    poll_set[0].events = POLLIN;
    int numfds = 1;

    char buffer[BUF_SIZE] = {0};

    while (numfds) {
        if (poll(poll_set, numfds, TIMEOUT) == -1) {
            perror("poll");
            return 2;
        }
        for (int fd_index = 0; fd_index < numfds; ++fd_index) {
            if (poll_set[fd_index].revents & POLLIN) {
                if (poll_set[fd_index].fd == server_fd) {
                    int clientSock = 0;
                    struct sockaddr clientAddr;
                    socklen_t clen = 0;
                    if ((clientSock = accept(server_fd, (struct sockaddr *) &clientAddr, &clen)) < 0) {
                        perror("accept");
                        return 4;
                    }
                    poll_set[numfds].fd = clientSock;
                    poll_set[numfds].events = POLLIN;
                    numfds++;
                    printf("Client %d connected\n", clientSock);
                } else {
                    int nread = 0;
                    ioctl(poll_set[fd_index].fd, FIONREAD, &nread);
                    if (nread == 0) {
                        close(poll_set[fd_index].fd);
                        --numfds;
                        printf("Client %d removed\n", poll_set[fd_index].fd);
                        poll_set[fd_index].events = 0;
                        poll_set[fd_index].fd = -1;

                    } else {
                        int strSize = 0;
                        if ((strSize = read(poll_set[fd_index].fd, buffer, BUF_SIZE)) == 0) {
                            perror("read");
                            return 5;
                        }
                        buffer[strSize] = 0;
                        printf("%s", buffer);
                    }
                }
            }
        }
    }
    return 0;
}