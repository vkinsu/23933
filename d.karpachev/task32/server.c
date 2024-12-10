#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_CLIENTS 5
#define SOCK_PATH "./unix_socket"
#define MAX_EVENTS 100

int main() {
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    int server_s;
    server_s = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCK_PATH);
    bind(server_s, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Waiting for clients...\n");
    fflush(stdout);
    listen(server_s, MAX_CLIENTS);


    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return 1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_s;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_s, &event) == -1) { 
        perror("epoll_ctl");
        return 1;
    }


    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            return 1;
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            if (fd == server_s) {
                struct sockaddr_un client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_s = accept(server_s, (struct sockaddr*)&client_addr, &client_len);
                if (client_s == -1) { 
                    perror("accept"); 
                    continue; 
                }

                printf("New client was connect.\n");

                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_s;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_s, &event);
            } 
            else {
                char buf[1024];
                ssize_t bcount = recv(fd, buf, sizeof(buf), 0);
                if (bcount <= 0) {
                    printf("Client was disconnect.\n");
                    close(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                } 
                else {
                    buf[bcount] = '\0';
                    printf("Recv: %s\n", buf);
                    for (size_t j = 0; j < bcount; ++j) {
                        buf[j] = toupper(buf[j]);
                    }
                    send(fd, buf, bcount, 0);
                }
            }
        }
    }

    close(server_s);
    unlink(SOCK_PATH);
    return 0;
}

