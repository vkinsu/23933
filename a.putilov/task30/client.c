#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_example_server"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) 
    {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server. Enter text (Ctrl+D to quit):\n");
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) 
        {
            perror("send");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    close(client_fd);
    printf("Client terminated.\n");
    return 0;
}
