#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>


#define SOCKET_PATH "/tmp/unix_socket"


void handle_client(int client_sock) {
    char buffer[256];
    ssize_t bytes_read;

    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = (char )toupper(buffer[i]);
        }
        printf("Received: %s\n", buffer);
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un addr;
    fd_set read_fds;
    int max_fd;

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);  // create Unix domain socket
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("failed to bind server sock");
        close(server_sock);
        return 1;
    }

    printf("wait for client..");  fflush(stdout);
    listen(server_sock, 5);
    printf("\n");

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_fd = server_sock;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("failed to select");
            break;
        }

        if (FD_ISSET(server_sock, &read_fds)) {
            client_sock = accept(server_sock, NULL, NULL);

            if (fork() == 0) {
                close(server_sock);
                handle_client(client_sock);
                return 0;
            }

            close(client_sock);
        }
    }

    unlink(SOCKET_PATH);
    close(server_sock);

    return 0;
}
