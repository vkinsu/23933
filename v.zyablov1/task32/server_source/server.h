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

void server_main_loop(int server_sock, int epoll_fd);

#endif // SERVER_H

