#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>

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
    listen(server_s, 1);

    socklen_t client_len = sizeof(client_addr);
    if ((client_s = accept(server_s, (struct sockaddr*)(&client_addr), &client_len)) < 0) {
        perror("Client connecting error!\n");
        return 1;
    }

    printf("Client was connect!\n");

    char buf[1024];
    while (1) {
        ssize_t bcount = recv(client_s, buf, 1024, 0);

        if (bcount <= 0) {
            printf("Client was disconnect.\n");
            break;
        }

        for (int i = 0; i < bcount; i++) {
            if (97 <= (int)buf[i] <= 122) {
                buf[i] = (char)toupper(buf[i]);
            }
        }

        printf("Upper recv: %s\n", buf);
    }

    close(client_s);
    close(server_s);
    unlink(SOCK_PATH);

    return 0;
}

