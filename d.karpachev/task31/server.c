#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>

#define MAX_CLIENTS 2
#define SOCK_PATH "./unix_socket"

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

    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = server_s;
    fds[0].events = POLLIN;
    int clients_count = 0;

    socklen_t client_addr_len = sizeof(client_addr);

    int poll_ret;
    int new_socket, refused_socket;
    char buf[1024];
    while (1) {
        poll_ret = poll(fds, clients_count + 1, -1);

        if (poll_ret < 0) {
            perror("Poll error!\n");
            return 1;
        }

        if ((fds[0].revents & POLLIN)) {

            if ((new_socket = accept(server_s, (struct sockaddr*)(&client_addr), &client_addr_len)) < 0) {
                perror("Accept failed!\n");
                continue;
            }
            printf("New client was connected.\n");
            fds[clients_count + 1].fd = new_socket;
            fds[clients_count + 1].events = POLLIN;
            clients_count++;
        }

        if (clients_count <= 0) {
            printf("All clients was disconnect.\n");
            break;
        }

        for (int i = 1; i <= clients_count; i++) {
            if (fds[i].revents & POLLIN) {
                ssize_t bcount = recv(fds[i].fd, buf, 1024, 0);
                if (bcount == 0) {
                    printf("Client was disconnect.\n");
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                else if (bcount > 0) {
                    for (int i = 0; i < bcount; i++) {
                        if (97 <= (int)buf[i] <= 122) {
                            buf[i] = (char)toupper(buf[i]);
                        }
                    }
                    buf[bcount] = '\0';
                    printf("Upper recv: %s\n", buf);
                }
                else {
                    perror("Recv failed!\n");
                }
            }
        }

        int j = 0;
        for (int i = 1; i <= clients_count; i++){
            if (fds[i].fd != -1){
                fds[j + 1] = fds[i];
                j++;
            }
        }
        clients_count = j;
    }

    close(server_s);
    unlink(SOCK_PATH);

    return 0;
}

