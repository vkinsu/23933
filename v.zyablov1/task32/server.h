#ifndef SERVER_H
#define SERVER_H

#define MAX_EVENTS 10
#define BUFFER_SIZE 256
#define SOCKET_PATH "/tmp/unix_socket"
#define RESPONSE_MESSAGE "Hello! You've activated the special message."

// Структура для хранения состояния клиента
typedef struct {
    int fd;
    int active;
} client_state_t;

int create_server_socket(const char *socket_path);
void accept_new_client(int server_sock, int epoll_fd);
void server_main_loop(int server_sock, int epoll_fd);
int setup_epoll(int server_sock);

#endif // SERVER_H

