#include "server.h"
#include "client_handler.h"
#include "setup_server.h"
#include "special_client_handling.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>    // Для epoll API: epoll_create1(), epoll_ctl(), epoll_wait()
#include <sys/socket.h>


// Главный цикл обработки событий
void server_main_loop(int server_sock, int epoll_fd) {
	int special_fd = eventfd(0, 0);  // Событие для уведомлений от сервера
	if (special_fd == -1) {
		perror("Failed to create eventfd");
		exit(1);
	}

	// Добавляем event_fd в epoll
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = special_fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, special_fd, &event) < 0) {
	    perror("Failed to add special_fd to epoll");
	    close(special_fd);
	    exit(1);
	}
	struct epoll_event events[MAX_EVENTS];

	while (1) {
		// Ожидание событий
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events < 0) {
			perror("epoll_wait failed");
			break;
		}

		for (int i = 0; i < num_events; i++) {
			int fd = events[i].data.fd;

			if (fd == server_sock) { // Новый клиент подключается
				accept_new_client(server_sock, epoll_fd);
			} else if (fd == special_fd) {
				handle_special_clients(fd, epoll_fd);
			} else { // Обработка старого клиента
				handle_client(fd, epoll_fd);
			}
		}
	}
}



// Основная функция сервера
int main() {
	int server_sock = create_server_socket(SOCKET_PATH);
	int epoll_fd = setup_epoll(server_sock);

	printf("Server is running...\n");
	server_main_loop(server_sock, epoll_fd);

	close(epoll_fd);
	close(server_sock);
	unlink(SOCKET_PATH);
	return 0;
}

