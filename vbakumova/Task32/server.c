#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <aio.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>  // Необходим для sigval_t и работы с уведомлениями

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256

// Функция для преобразования текста в верхний регистр
void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

// Функция для получения текущего времени в строковом формате
void get_current_time(char *time_str, size_t len) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(time_str, len, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Обработчик для завершения асинхронной операции
void aio_completion_handler(union sigval sigval) {
    struct aiocb *cb = (struct aiocb *)sigval.sival_ptr;
    int client_fd = cb->aio_fildes;
    
    // Приведение cb->aio_buf к volatile указателю
    char *buffer = (char *)cb->aio_buf;
    ssize_t bytes_read = aio_return(cb);

    if (bytes_read > 0) {
        // Завершаем строку нулевым символом
        buffer[bytes_read] = '\0';
        to_uppercase((char *)buffer);  // Преобразуем текст в верхний регистр

        // Получаем текущее время
        char time_str[20];
        get_current_time(time_str, sizeof(time_str));

        // Печатаем преобразованный текст с временем
        printf("[%s] %s", time_str, buffer);

        // Запускаем асинхронное чтение снова для дальнейших данных от клиента
        // Приведение cb->aio_buf к volatile указателю
        memset((void *)cb->aio_buf, 0, BUFFER_SIZE);  // Очистим буфер
        aio_read(cb);
    } else if (bytes_read == 0) {
        printf("Client disconnected\n");
        close(client_fd);  // Закрываем соединение с клиентом
        free((void *)buffer);  // Освобождаем память для буфера
        free(cb);  // Освобождаем структуру aio
    } else {
        perror("Error reading from client");
        close(client_fd);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    socklen_t client_len;

    // Создаем сокет
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Error: Socket creation failed");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);  // Удаляем старый файл сокета, если он существует

    // Настройки адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error: Binding failed");
        exit(EXIT_FAILURE);
    }

    // Переводим сокет в режим прослушивания
    if (listen(server_fd, 10) == -1) {
        perror("Error: Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    while (1) {
        client_len = sizeof(server_addr);

        // Принимаем новое подключение
        if ((client_fd = accept(server_fd, (struct sockaddr *)&server_addr, &client_len)) == -1) {
            perror("Error: Accept failed");
            continue;
        }

        // Выделяем память для буфера и структуры aio
        char *buffer = (char *)malloc(BUFFER_SIZE);
        struct aiocb *cb = (struct aiocb *)malloc(sizeof(struct aiocb));

        // Настроим структуру aiocb
        memset(cb, 0, sizeof(struct aiocb));
        cb->aio_fildes = client_fd;
        cb->aio_buf = buffer;
        cb->aio_nbytes = BUFFER_SIZE - 1;
        cb->aio_offset = 0;
        cb->aio_sigevent.sigev_notify = SIGEV_THREAD;
        cb->aio_sigevent.sigev_notify_function = aio_completion_handler;
        cb->aio_sigevent.sigev_notify_attributes = NULL;
        cb->aio_sigevent.sigev_value.sival_ptr = cb;

        // Инициируем асинхронное чтение
        if (aio_read(cb) == -1) {
            perror("Error starting aio_read");
            free(buffer);
            free(cb);
            close(client_fd);
        }
    }

    // Закрываем серверный сокет
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
