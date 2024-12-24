#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

// Обёртка для создания сокета
int create_client_socket() {
	int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_sock < 0) {
		perror("Failed to create socket");
		exit(1);
	}
	return client_sock;
}

// Обёртка для подключения к серверу
void connect_to_server(int client_sock, const char *socket_path) {
	struct sockaddr_un server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

	printf("Connecting..."); fflush(stdout);

	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Failed to connect to server");
		close(client_sock);
		exit(1);
	}

	printf(".\tConnected!\n");
}

// Обёртка для отправки сообщения
void send_message(int client_sock, const char *message) {
	ssize_t sent_len = send(client_sock, message, strlen(message), 0);
	if (sent_len < 0) {
		perror("Failed to send message");
		close(client_sock);
		exit(1);
	}
}

void handle_server_connection(int client_sock) {
	char buffer[256];
	
	// Цикл для ввода сообщений и их отправки
	while (1) {
		printf("Client: ");
		fgets(buffer, sizeof(buffer), stdin);

		// Убираем символ новой строки из ввода
		buffer[strcspn(buffer, "\n")] = '\0';

		// Если введено "q", завершаем программу
		if (strcmp(buffer, "q") == 0) {
			break;
		}

		// Отправляем сообщение серверу
		send_message(client_sock, buffer);
	}
}

void auto_server_connection(int client_sock) {
    const char *message = "spam message";
    while (1) {
        printf("Auto Client: %s\n", message);
        send_message(client_sock, message);
        sleep(2);  // Имитация задержки между сообщениями
    }
}


// Основная функция клиента
int main(int argc, char *argv[]) {
    // Проверка аргументов
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--auto") != 0)) {
        printf("Использование: %s [--auto]\n", argv[0]);
        return 1;
    }

    // Создаём сокет
    const char *socket_path = "/tmp/unix_socket";
    int client_sock = create_client_socket();

    // Подключаемся к серверу
    connect_to_server(client_sock, socket_path);

    // Выбор режима работы
    if (argc == 2 && strcmp(argv[1], "--auto") == 0) {
        auto_server_connection(client_sock);
    } else {
        handle_server_connection(client_sock);
    }

    // Закрываем сокет
    close(client_sock);

    return 0;
}

