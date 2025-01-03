#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/time.h>

#define SOCK_PATH "./unix_socket"

int main() {
    int client_s;
    client_s = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    printf("Connecting to the server...\n");
    fflush(stdout);

    if (connect(client_s, (struct sockaddr*)(&server_addr), sizeof(server_addr)) < 0) {
        perror("Error while connecting!\n");
        return 1;
    }

    printf("Succesfully connected!\n");

    char buf[1024];
    while (1) {
        printf("Input: ");
        fgets(buf, 1024, stdin);

        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "q") == 0) {
            break; 
        }

        send(client_s, buf, strlen(buf), 0);

        usleep(1000000 * strlen(buf));

        ssize_t bytes_received = recv(client_s, buf, 1024, 0);
        if (bytes_received > 0) {
            buf[bytes_received] = '\0';
            printf("Server response: %s\n", buf);
        } 
        else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        } 
        else {
            perror("Error receiving data from server");
            break;
        }
    }

    close(client_s);

    return 0;
}

