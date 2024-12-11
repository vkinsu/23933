#include "server.h"
#include "client_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/epoll.h>

// Создание серверного сокета
int create_server_socket(const char *socket_path) {
    struct sockaddr_un server_addr;

    // 1. Создание сокета
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Failed to create server socket");
        exit(1);
    }

    // 2. Удаление старого файла сокета, если он существует
    unlink(socket_path);

    // 3. Заполнение структуры адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    // 4. Привязка сокета к файловому пути
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        close(server_sock);
        exit(1);
    }

    // 5. Перевод сокета в режим прослушивания
    if (listen(server_sock, SOMAXCONN) < 0) {
        perror("Failed to listen on socket");
        close(server_sock);
        exit(1);
    }

    printf("Server socket created and listening at %s\n", socket_path);
    return server_sock;
}

// Обработка нового подключения клиента
void accept_new_client(int server_sock, int epoll_fd) {
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Failed to accept new client");
        return;
    }

    printf("New client connected: %d\n", client_sock);

    // Добавление клиента в epoll
    struct epoll_event event;
    event.events = EPOLLIN;  // Событие: готовность к чтению
    event.data.fd = client_sock;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) < 0) {
        perror("Failed to add client socket to epoll");
        close(client_sock);
    }
}

// Главный цикл обработки событий
void server_main_loop(int server_sock, int epoll_fd) {
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        // Ожидание событий
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_sock) { // Новый клиент подключается
                accept_new_client(server_sock, epoll_fd);
            } else { // Обработка старого клиента
                handle_client(fd, epoll_fd);
            }
        }
    }
}

// Создание epoll и добавление серверного сокета
int setup_epoll(int server_sock) {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("Failed to create epoll instance");
        close(server_sock);
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN;  // Готовность к чтению
    event.data.fd = server_sock;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event) < 0) {
        perror("Failed to add server socket to epoll");
        close(server_sock);
        close(epoll_fd);
        exit(1);
    }

    printf("Epoll setup complete\n");
    return epoll_fd;
}

// Основная функция сервера
int main() {
    int server_sock = create_server_socket(SOCKET_PATH);
    int epoll_fd = setup_epoll(server_sock);

    printf("Server is running...\n");
    server_main_loop(server_sock, epoll_fd);

    close(epoll_fd);
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}

