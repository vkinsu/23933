#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "pthread_data.h" 
#define SOCKET_PATH "/tmp/unix_socket"
#define NUM_CLIENTS 3

// Глобальные переменные для сигналов, завершающих отправку сообщений
volatile sig_atomic_t servers_done[NUM_CLIENTS] = {0};

/**
* Обработчик сигнала завершения, устанавливает флаг.
*/
void server_done_handler(int sig) {
	if (sig == SIGRTMIN + 2) {
		servers_done[0] = 1;  // Клиент 0 завершён
	} else if (sig == SIGRTMIN + 5) {
		servers_done[1] = 1;  // Клиент 1 завершён
	}
}

/**
* Создаёт сокет и подключается к серверу по UNIX-сокету.
* Возвращает файловый дескриптор сокета или завершает выполнение при ошибке.
*/
int connect_to_server() {
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("Socket error");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("Connect error");
		close(fd);
		exit(EXIT_FAILURE);
	}

	return fd;
}

/**
* Отправляет сообщение серверу с использованием асинхронного ввода-вывода.
* Ожидает полного завершения передачи.
*/
void send_message(int client_fd, int client_id, const char* message) {
        pthread_data* pd = (pthread_data*)arg;
        int sig = SIGRTMIN + pd->id * 3;

	struct aiocb writerq;
	memset(&writerq, 0, sizeof(writerq));
	writerq.aio_fildes = client_fd;
	writerq.aio_buf = (void*)message;
	writerq.aio_nbytes = strlen(message);

	printf("Client %d sending: %s\n", client_id, message);

	if (aio_write(&writerq) == -1) {
		perror("aio_write error");
		close(client_fd);
		exit(EXIT_FAILURE);
	}
	sigsend(P_PID, pd->ppid, sig);  // отправка сигнала после начала асинхронной операции
	// Ждём завершения асинхронной отправки
	const struct aiocb* requests[1] = {&writerq};
	while (aio_suspend(requests, 1, NULL) == 0) {
		if (aio_error(&writerq) == 0) {
			printf("Client %d finished sending\n", client_id);
			break;
		}
	}
	sigsend(P_PID, pd->ppid, sig + 1); // отправка сигнала об завершении асинхронной операции

	// Ожидание подтверждения завершения со стороны сервера
	while (!servers_done[client_id]);

	printf("Client %d disconnecting\n", client_id);
	close(client_fd);
}

/**
* Устанавливает обработчики сигналов.
*/
void set_signal_handlers() {
	signal(SIGRTMIN + 2, server_done_handler);  // Завершение для клиента 0
	signal(SIGRTMIN + 5, server_done_handler);  // Завершение для клиента 1
}

int main() {
	set_signal_handlers();

	// Подключаемся к серверу как два клиента
	int client_fd1 = connect_to_server();
	int client_fd2 = connect_to_server();

	// Отправляем сообщения серверу
	send_message(client_fd1, 0, "Hello world!");
	send_message(client_fd2, 1, "Another message");

	printf("Client program finished\n");
	return 0;
}

