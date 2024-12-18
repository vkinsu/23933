#include "client_handler.h"

#include <sys/socket.h>   // Для socket(), bind(), listen(), send(), recv()
#include <sys/un.h>       // Для sockaddr_un и AF_UNIX
#include <sys/epoll.h>    // Для epoll API: epoll_create1(), epoll_ctl(), epoll_wait()
#include <unistd.h>       // Для close(), read(), unlink()
#include <errno.h>        // Для ошибок errno
#include <string.h>       // Для strlen()
#include <stdlib.h>       // Для malloc(), free() и exit()
#include <stdio.h>        // Для printf(), perror()
#include <ctype.h>        // Для toupper()


/*
int check_for_keyword(const char *buffer, const char *keyword) {
	return strstr(buffer, keyword) != NULL;
}
*/
// Функция для обработки текста
void process_text(char *buffer, ssize_t length) {
	for (int i = 0; i < length; i++) {
		buffer[i] = (char)toupper((unsigned char)buffer[i]);
	}
}

// Обработчик клиента
void handle_client(int client_sock, int epoll_fd) {
	char buffer[BUFFER_SIZE];

	ssize_t bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
	if (bytes_received <= 0) {
		// Если клиент отключился
		printf("Client disconnected: %d\n", client_sock);
		close(client_sock);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
	}
	else if (strncmp(buffer, "quit", 4) == 0) {
		printf("Client requested disconnect: %d\n", client_sock);
		close(client_sock);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
	} else {
		buffer[bytes_received] = '\0';
		process_text(buffer, bytes_received);
		printf("Received from client %d: %s\n", client_sock, buffer);

		// Эхо-ответ клиенту
		send(client_sock, buffer, bytes_received, 0);
		/*  Блок ключевого слова
		// Если клиент отправил ключевое слово "keyword"
		if (strstr(buffer, "keyword") != NULL) {
			// Запускаем отправку фразы с интервалом 4 секунды
			const char *message = "Server response: keyword detected!";
			while (1) {
				// Отправляем сообщение
				send(client_sock, message, strlen(message), 0);
				sleep(4);  // Задержка 4 секунды
			}
		} else {
			// Эхо-ответ клиенту (если нет ключевого слова)
			send(client_sock, buffer, bytes_received, 0);
		}
		*/
	}
}

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
