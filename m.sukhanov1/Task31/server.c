#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/unix_sock"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

int main() {
    int server_sock, client_sock, max_sd, activity, new_socket;
    int client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE] = {0};
    fd_set readfds;

    // Создание серверного сокета
    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(1);
    }

    // Настройка адреса сервера
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Удаление старого файла сокета, если он существует
    unlink(SOCKET_PATH);

    // Привязка сокета к адресу
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_sock);
        exit(2);
    }

    // Прослушивание входящих подключений
    if (listen(server_sock, 5) == -1) {
        perror("listen failed");
        close(server_sock);
        exit(3);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        // Очистка набора дескрипторов
        FD_ZERO(&readfds);

        // Добавление серверного сокета в набор
        FD_SET(server_sock, &readfds);
        max_sd = server_sock;

        // Добавление клиентских сокетов в набор
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Ожидание активности на любом из сокетов
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
        }

        // Новое входящее подключение
        if (FD_ISSET(server_sock, &readfds)) {
            if ((new_socket = accept(server_sock, NULL, NULL)) < 0) {
                perror("accept failed");
                exit(4);
            }

            printf("New connection on socket %d\n", new_socket);

            // Добавление нового клиента в массив
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("Added client to slot %d\n", i);
                    break;
                }
            }
        }

        // Проверка активности на клиентских сокетах
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                ssize_t n = recv(sd, buffer, BUFFER_SIZE, 0);
                if (n == 1 && buffer[0] == '\0') { // Клиент завершил соединение
                    printf("Client on socket %d disconnected\n", sd);
                    close(sd);
                    client_sockets[i] = 0;
                } else if (n > 0) {
                    buffer[n] = '\0';
                    printf("Received from client %d: %s\n", sd, buffer);

                    // Преобразование в верхний регистр
                    for (int j = 0; j < n; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }

                    // Отправка данных обратно клиенту
                    send(sd, buffer, n, 0);
                } else if (n == 0) {
                    printf("Client on socket %d disconnected\n", sd);
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    perror("recv failed");
                    close(sd);
                    client_sockets[i] = 0;
                }
            }
        }
    }

    close(server_sock);
    unlink(SOCKET_PATH);

    return 0;
}

