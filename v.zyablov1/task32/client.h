#ifndef CLIENT_H
#define CLIENT_H

#define SOCKET_PATH "/tmp/unix_socket"
// Если введено "q", завершаем программу
/*
if (strcmp(buffer, "q") == 0) {
    printf("Exiting...\n");
    exit_flag_custom = 1;
}*/
// volatile int exit_flag_custom = 0;  // Глобальная переменная для завершения работы

// Создание неблокирующего сокета
int create_nonblocking_socket();

// Подключение к серверу
void connect_to_server(int client_sock, const char *socket_path);

// Отправка сообщения
void send_message(int client_sock, const char *message);

// Обработка активности сокета
void handle_socket_activity(int client_sock);

// Обработка ввода пользователя
void handle_input_activity(int client_sock);

// Основной цикл асинхронного клиента
void async_client_main_loop(int client_sock);

// Основной цикл автоматического клиента
void async_auto_client_main_loop(int client_sock);

#endif // CLIENT_H

