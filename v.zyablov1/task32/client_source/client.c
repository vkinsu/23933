#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/select.h>

int create_nonblocking_socket() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        exit(1);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL) failed");
        exit(1);
    }

    return sock;
}

void connect_to_server(int client_sock, const char *socket_path) {
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    printf("Connecting..."); fflush(stdout);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        close(client_sock);
        exit(1);
    }

    printf(".\tConnected!\n");
}

void send_message(int client_sock, const char *message) {
    ssize_t sent_len = send(client_sock, message, strlen(message), 0);
    if (sent_len < 0) {
        perror("Failed to send message");
        close(client_sock);
        exit(1);
    }
}

void handle_socket_activity(int client_sock) {
    char buffer[256];
    ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    } else if (bytes_received == 0) {
        printf("Server closed the connection.\n");
        exit(0);
    } else {
        perror("recv() failed");
        exit(1);
    }
}

void handle_input_activity(int client_sock) {
    char buffer[256];
    printf("Enter message: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (send(client_sock, buffer, strlen(buffer), 0) < 0) {
            perror("send() failed");
        }
    }
}

void async_client_main_loop(int client_sock) {
    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(client_sock, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(client_sock + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select() failed");
            break;
        } else if (activity == 0) {
            printf("No activity in the last 5 seconds.\n");
            continue;
        }

        if (FD_ISSET(client_sock, &read_fds)) {
            handle_socket_activity(client_sock);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            handle_input_activity(client_sock);
        }
    }
}

void async_auto_client_main_loop(int client_sock) {
    fd_set read_fds;
    struct timeval timeout;
    const char *message = "spam message";

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(client_sock, &read_fds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(client_sock + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select() failed");
            break;
        } else if (activity == 0) {
            printf("Auto Client: %s\n", message);
            if (send(client_sock, message, strlen(message), 0) < 0) {
                perror("send() failed");
                break;
            }
            continue;
        }

        if (FD_ISSET(client_sock, &read_fds)) {
            handle_socket_activity(client_sock);
        }
    }
}

int main(int argc, char *argv[]) {
    // Проверка аргументов
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--auto") != 0)) {
        printf("Использование: %s [--auto]\n", argv[0]);
        return 1;
    }
    int client_sock = create_nonblocking_socket();
    connect_to_server(client_sock, SOCKET_PATH);
    // Выбор режима работы
    if (argc == 2 && strcmp(argv[1], "--auto") == 0) {
        async_auto_client_main_loop(client_sock);
    } else {
        async_client_main_loop(client_sock);
    }
    close(client_sock);
    return 0;
}
