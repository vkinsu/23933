#include "setup_server.h"

#include <sys/socket.h>   // Для socket(), bind(), listen(), send(), recv()
#include <sys/un.h>       // Для sockaddr_un и AF_UNIX
#include <sys/epoll.h>    // Для epoll API: epoll_create1(), epoll_ctl(), epoll_wait()
#include <unistd.h>       // Для close(), read(), unlink()
#include <errno.h>        // Для ошибок errno
#include <string.h>       // Для strlen()
#include <stdlib.h>       // Для malloc(), free() и exit()
#include <stdio.h>        // Для printf(), perror()


int create_server_socket(const char *socket_path) {
	struct sockaddr_un server_addr;

	// 1. Создание сокета
	int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("Failed to create server socket");
		exit(1);
	}

	// 2. Удаление старого файла сокета, если он существует
	unlink(socket_path);

	// 3. Заполнение структуры адреса сервера
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

	// 4. Привязка сокета к файловому пути
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Failed to bind socket");
		close(server_sock);
		exit(1);
	}

	// 5. Перевод сокета в режим прослушивания
	if (listen(server_sock, SOMAXCONN) < 0) {
		perror("Failed to listen on socket");
		close(server_sock);
		exit(1);
	}

	printf("Server socket created and listening at %s\n", socket_path);
	return server_sock;
}


int setup_epoll(int server_sock) {
	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("Failed to create epoll instance");
		close(server_sock);
		exit(1);
	}

	struct epoll_event event;
	event.events = EPOLLIN;  // Готовность к чтению
	event.data.fd = server_sock;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event) < 0) {
		perror("Failed to add server socket to epoll");
		close(server_sock);
		close(epoll_fd);
		exit(1);
	}

	printf("Epoll setup complete\n");
	return epoll_fd;
}
