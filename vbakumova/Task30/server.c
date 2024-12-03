#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>


#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256

int main(){
    int server_fd, client_fd;
    struct sockaddr_un server_addr;

    // settings for socket's addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path));

    unlink(SOCKET_PATH);

    // ctreature socket
    if((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("Error: Socket creation failed.");
        exit(EXIT_FAILURE);
    }
    
    // binding the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("Error: Binding failed.");
        exit(EXIT_FAILURE);
    }

    // change rejim to "connecting"
    if (listen(server_fd, 5) == -1) {
        perror("Error: listening failed.");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s...\n", SOCKET_PATH);

    // Getting  onnect from client
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("Accepted");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char buf[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(client_fd, buf, sizeof(buf) -1)) > 0){
        for (ssize_t i = 0; i < bytes_read; i++) {
            buf[i] = toupper(buf[i]);
        }
        printf("Received and converted: %c\n", buf);
    }

    // close connectong
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH); // delete socket's file

    return 0;
}