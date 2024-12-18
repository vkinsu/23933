#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aio.h>
#include <errno.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/unix_socket"
#define MAX_CLIENTS 3

// Флаги для контроля начала и завершения работы с клиентами.
volatile sig_atomic_t beg_messages[MAX_CLIENTS] = {0};
volatile sig_atomic_t end_messages[MAX_CLIENTS] = {0};

void clear_volatile(volatile sig_atomic_t *array, size_t size) {
	for (size_t i = 0; i < size / sizeof(int); i++) {
		array[i] = 0;
	}
}


/**
* Настройка и возврат файлового дескриптора UNIX-сокета.
*/
int setup_server_socket() {
	int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd == -1) {
		perror("Socket error");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
	unlink(SOCKET_PATH);

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("Bind error");
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (listen(fd, MAX_CLIENTS) == -1) {
		perror("Listen error");
		close(fd);
		exit(EXIT_FAILURE);
	}

	return fd;
}

/**
* Обработчик сигнала: установка флага начала при SIGRTMIN + X.
*/
void begin_handler(int sig) {
	if (sig == SIGRTMIN) {
		beg_messages[0] = 1;
	} else if (sig == SIGRTMIN + 3) {
		beg_messages[1] = 1;
	}
}

/**
* Обработчик сигнала: установка флага завершения при SIGRTMIN + X + 1.
*/
void end_handler(int sig) {
	if (sig == SIGRTMIN + 1) {
		end_messages[0] = 1;
	} else if (sig == SIGRTMIN + 4) {
		end_messages[1] = 1;
	}
}

/**
* Устанавливает обработчики сигналов для начала и завершения.
*/
void set_reactions() {
	for (int id = 0; id < MAX_CLIENTS; id++) {
		int sig = SIGRTMIN + id * 3;
		signal(sig, begin_handler);       // Обработка начала сообщения
		signal(sig + 1, end_handler);     // Обработка завершения сообщения
	}
}

/**
* Создаёт процесс клиента и запускает исполняемый файл `./client`.
*/
pid_t create_client() {
	pid_t pid = fork();
	if (pid == -1) {
		perror("Fork error");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		execl("./client", "./client", NULL); // Запускаем клиент
		perror("exec error");
		exit(EXIT_FAILURE);
	}

	return pid;
}

/**
* Читает сообщения от клиента с использованием асинхронного ввода-вывода.
*/
void process_client(int client_fd, int client_id) {
	struct aiocb readrq;
	memset(&readrq, 0, sizeof(readrq));

	char sym = 0;
	readrq.aio_fildes = client_fd;
	readrq.aio_buf = &sym;
	readrq.aio_nbytes = 1;

	// Ждём сигнала о начале чтения
	while (!beg_messages[client_id]);

	printf("Server starts reading client %d\n", client_id);

	aio_read(&readrq);
	while (1) {
		int ret = aio_error(&readrq);
		if (ret == 0) {
			// Чтение успешно завершено
			putchar(toupper(sym));
			aio_read(&readrq); // Читаем следующий символ
		} else if (ret == EAGAIN && end_messages[client_id]) {
			// Конец передачи
			printf("\nServer finished reading client %d\n", client_id);
			close(client_fd);
			return;
		} else if (ret != EINPROGRESS) {
			perror("aio read error");
			close(client_fd);
			return;
		}
	}
}

/**
* Главная функция сервера.
* 1. Настраивает сокет.
* 2. Запускает клиентов.
* 3. Принимает соединения и обрабатывает данные от клиентов.
*/
int main() {
	int server_fd = setup_server_socket(); // Создаём серверный сокет
	set_reactions();                      // Устанавливаем обработчики сигналов

	pid_t client_pid = create_client();   // Запускаем клиента

	int client_fds[MAX_CLIENTS] = {0};    // Дескрипторы клиентских соединений
	clear_volatile(beg_messages, sizeof(beg_messages));
	clear_volatile(end_messages, sizeof(end_messages));

	int connected_clients = 0;
	while (connected_clients < MAX_CLIENTS) {
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd == -1) {
			continue; // Если клиент не подключился, продолжаем ожидание
		}

		client_fds[connected_clients] = client_fd;

		// Обрабатываем клиента в порядке подключения
		process_client(client_fd, connected_clients);
		connected_clients++;
	}

	for (int i = 0; i < connected_clients; i++) {
		close(client_fds[i]);
	}

	waitpid(client_pid, NULL, 0); // Ждём завершения работы клиента
	close(server_fd);
	printf("Server finished.\n");
	return 0;
}

