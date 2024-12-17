#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/select.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

int main() {
    int server_fd, client_fd, max_fd, activity;
    struct sockaddr_un server_addr;
    fd_set read_fds, master_fds;

    // Создаем сокет
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Error: Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    // Настройки адреса сокета
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: Binding failed.");
        exit(EXIT_FAILURE);
    }

    // Переводим сокет в режим ожидания подключений
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Error: Listening failed.");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s...\n", SOCKET_PATH);

    // Инициализация множеств дескрипторов
    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    while (1) {
        read_fds = master_fds;

        // Ждем активности на сокетах
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error: Select failed.");
            break;
        }

        // Проверяем, есть ли новые подключения
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                perror("Error: Accept failed.");
            } else {
                printf("New client connected.\n");
                FD_SET(client_fd, &master_fds);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
            }
        }

        // Проверяем активность на других дескрипторах
        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_fds) && fd != server_fd) {
                char buf[BUFFER_SIZE];
                ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);

                if (bytes_read > 0) {
                    // Преобразуем текст в верхний регистр
                    for (ssize_t i = 0; i < bytes_read; i++) {
                        buf[i] = toupper(buf[i]);
                    }
                    buf[bytes_read] = '\0';

                    // Выводим преобразованный текст
                    printf("%s", buf);
                } else {
                    // Клиент закрыл соединение
                    printf("Client disconnected.\n");
                    close(fd);
                    FD_CLR(fd, &master_fds);
                }
            }
        }
    }

    // Закрываем сервер и удаляем файл сокета
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
