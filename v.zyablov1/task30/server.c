#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

// Функция для создания Unix доменного сокета
int create_server_socket(const char *socket_path) {
	struct sockaddr_un server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

	int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("Failed to create server socket");
		exit(1);
	}

	unlink(socket_path);  // Удаляем старый сокет, если существует

	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Failed to bind socket");
		exit(1);
	}

	return server_sock;
}

int wait_for_client(int server_sock) {
	listen(server_sock, 1); // Слушаем одно входящее соединение

	// Принимаем соединение
	struct sockaddr_un client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
	
	if (client_sock < 0) {
		perror("Failed to accept client connection");
		exit(-1);
	}
	return client_sock;
}

// Функция для обработки соединения с клиентом
void handle_client_connection(int client_sock) {
	char buffer[256];

	while (1) {
		ssize_t bytes_received = recv(client_sock, buffer, 256, 0);
		if (bytes_received <= 0) {break; }  // Завершаем цикл, если клиент отключился или произошла ошибка

		buffer[bytes_received] = '\0';  // Завершаем строку

		// Переводим текст в верхний регистр
		for (int i = 0; i < bytes_received; i++) {
			buffer[i] = (char)toupper((unsigned char)buffer[i]);
		}
		// Выводим полученный текст
		printf("Received: %s\n", buffer);
	}
}

// Основная функция сервера
int main() {
	const char *socket_path = "/tmp/unix_socket";
	int server_sock = create_server_socket(socket_path);

	printf("Waiting for client...\n");
	fflush(stdout); // необходим для немедленного вывода сообщения на экран, обходя буферизацию вывода.
	int client_sock = wait_for_client(server_sock);

	printf("Client connected!\n");
	handle_client_connection(client_sock);  // Обрабатываем соединение с клиентом

	close(client_sock);
	close(server_sock);
	unlink(socket_path);  // Удаляем сокет по завершению
	return 0;
}

