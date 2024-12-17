#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./my_socket"

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un addr;
    char buf[1024];

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(1);
    }
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_sock, 1) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Waiting for the client to connect...\n");

    client_sock = accept(server_sock, NULL, NULL);
    if (client_sock < 0) {
        perror("accept");
        exit(1);
    }

    printf("The client is connected.\n");

    ssize_t num_read = read(client_sock, buf, sizeof(buf) - 1);
    if (num_read > 0) {
        buf[num_read] = '\0';
        to_uppercase(buf);
        printf("Received a message from a client: %s\n", buf);
    } else {
        perror("read");
    }

    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
