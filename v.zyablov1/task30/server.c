#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>


int main() {
    struct sockaddr_un server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "/tmp/unix_socket", sizeof(server_addr.sun_path) - 1);

    int server_sock, client_sock;
    socklen_t client_len;
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0); // create Unix domain socket
    unlink("/tmp/unix_socket");  // unlink old socket

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("failed to bind socket");
        return 1;
    }

    printf("wait for client..");  fflush(stdout);
    listen(server_sock, 1);

    client_len = sizeof(client_addr);
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("failed to get client connection");
        return 1;
    }

    printf(".\tconnected!\n");

    char buffer[256];

    while (1) {
        ssize_t bytes_received = recv(client_sock, buffer, 256, 0);

        if (bytes_received <= 0) {break; }  // client disconnected

        buffer[bytes_received] = '\0';

        for (int i = 0; i < bytes_received; i++) {
            buffer[i] = (char )toupper(buffer[i]);
        }

        printf("received: %s\n", buffer);
    }

    close(client_sock);
    close(server_sock);
    unlink("/tmp/unix_socket");

    return 0;
}
