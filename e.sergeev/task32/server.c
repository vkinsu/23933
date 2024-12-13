#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <aio.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_server"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int fd;
    struct aiocb aio_req;
    char buffer[BUFFER_SIZE];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

// Прототипы функций
void start_reading(Client *client);
void handle_client_read(union sigval sigval);
void to_uppercase(char *str);

// Перевод строки в верхний регистр
void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

// Начать асинхронное чтение для клиента
void start_reading(Client *client) {
    memset(&client->aio_req, 0, sizeof(client->aio_req));
    client->aio_req.aio_fildes = client->fd;
    client->aio_req.aio_buf = client->buffer;
    client->aio_req.aio_nbytes = BUFFER_SIZE;
    client->aio_req.aio_sigevent.sigev_notify = SIGEV_THREAD;
    client->aio_req.aio_sigevent.sigev_notify_function = handle_client_read;
    client->aio_req.aio_sigevent.sigev_value.sival_ptr = client;

    if (aio_read(&client->aio_req) == -1) {
        perror("aio_read");
        close(client->fd);
        return;
    }
}

// Обработчик завершения чтения
void handle_client_read(union sigval sigval) {
    Client *client = (Client *)sigval.sival_ptr;
    int bytes_read = aio_return(&client->aio_req);

    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("Client disconnected.\n");
        } else {
            perror("aio_return");
        }
        close(client->fd);
        *client = clients[--client_count];
        return;
    }

    client->buffer[bytes_read] = '\0';
    to_uppercase(client->buffer);
    printf("Received and transformed: %s\n", client->buffer);

    start_reading(client);
}

// Основная функция работы сервера
void run_server() {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        if (client_count >= MAX_CLIENTS) {
            printf("Maximum clients reached. Connection refused.\n");
            close(client_fd);
            continue;
        }

        clients[client_count].fd = client_fd;
        printf("New client connected.\n");
        start_reading(&clients[client_count]);
        client_count++;
    }

    close(server_fd);
    unlink(SOCKET_PATH);
}

// Точка входа
int main() {
    printf("Starting server...\n");
    run_server();
    return 0;
}

