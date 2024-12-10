#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

// Функция для создания Unix доменного сокета
int create_server_socket(const char *socket_path) {
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Failed to create server socket");
        exit(1);
    }

    unlink(socket_path); // Удаляем старый сокет, если существует

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        close(server_sock);
        exit(1);
    }

    return server_sock;
}

// Функция для обработки текста
void process_text(char *buffer, ssize_t length) {
    for (int i = 0; i < length; i++) {
        buffer[i] = (char)toupper((unsigned char)buffer[i]);
    }
}

// Функция для обработки подключения нового клиента
void accept_new_client(int server_sock, fd_set *active_fds, int *max_fd) {
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Failed to accept new client");
        return;
    }

    printf("New client connected: %d\n", client_sock);

    // Добавляем новый клиентский сокет в множество активных дескрипторов
    FD_SET(client_sock, active_fds);

    // Обновляем максимальный дескриптор
    if (client_sock > *max_fd) {
        *max_fd = client_sock;
    }
}

// Функция для обработки одного клиента
void handle_client(int client_sock, fd_set *active_fds) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        // Клиент отключился
        printf("Client %d disconnected\n", client_sock);
        close(client_sock);
        FD_CLR(client_sock, active_fds);
    } else {
        buffer[bytes_received] = '\0';
        process_text(buffer, bytes_received);
        printf("From client %d: %s\n", client_sock, buffer);

        // Отправляем преобразованный текст обратно клиенту
        send(client_sock, buffer, bytes_received, 0);
    }
}

// Главный цикл работы сервера
void run_server(int server_sock) {
    fd_set active_fds, read_fds; // множество активных файловых дескрипторов для отслеживания с помощью select.
    FD_ZERO(&active_fds);
    FD_SET(server_sock, &active_fds);

    int max_fd = server_sock; // максимальный номер дескриптора для обновления в случае подключения нового клиента.

    while (1) {
        read_fds = active_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            /*
            max_fd + 1 = Максимальный номер дескриптора в проверяемых множествах плюс единица.
            &read_fds = Указатель на множество дескрипторов, которые проверяются на возможность чтения (например, новых подключений или данных).
            NULL (второй аргумент) = Множество дескрипторов для проверки возможности записи.
            NULL (третий аргумент) = Множество дескрипторов для проверки на наличие исключений.
            NULL (четвёртый аргумент) = Таймаут для завершения вызова select. Если NULL, ожидание продолжается бесконечно до события.
            */
            perror("select() failed");
            break;
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == server_sock) { // Новый клиент подключается
                    accept_new_client(server_sock, &active_fds, &max_fd);
                } else { // Обрабатываем старого клиента
                    handle_client(fd, &active_fds);
                }
            }
        }
    }
}

int main() {
    const char *socket_path = "/tmp/unix_socket";
    int server_sock = create_server_socket(socket_path);

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Failed to listen on socket");
        close(server_sock);
        unlink(socket_path);
        exit(1);
    }

    printf("Server is running and waiting for connections...\n");
    run_server(server_sock);

    close(server_sock);
    unlink(socket_path);
    return 0;
}

