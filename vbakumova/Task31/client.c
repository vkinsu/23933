#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un server_addr;

    // ctreature socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // settings for socket's addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // connecting to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];

    // getting text
    printf("Enter text to send to the server: ");
    fgets(buffer, sizeof(buffer), stdin);

    // sending data to server
    if (write(client_fd, buffer, strlen(buffer)) == -1) {
        perror("write");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    while(1){

        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    // close connecting
    close(client_fd);
    return 0;
}
