#include "special_client_handling.h"

#include <sys/socket.h>   // Для socket(), bind(), listen(), send(), recv()
#include <sys/un.h>       // Для sockaddr_un и AF_UNIX
#include <sys/epoll.h>    // Для epoll API: epoll_create1(), epoll_ctl(), epoll_wait()
#include <unistd.h>       // Для close(), read(), unlink()
#include <errno.h>        // Для ошибок errno
#include <string.h>       // Для strlen()
#include <stdlib.h>       // Для malloc(), free() и exit()
#include <stdio.h>        // Для printf(), perror()


#define MAX_CLIENTS 10
bool special_clients[MAX_CLIENTS] = {false}; // Инициализация

void mark_client_as_special(int client_fd) {
    special_clients[client_fd] = true;
}

bool is_special_client(int client_fd) {
    return special_clients[client_fd];
}

void notify_special_clients(int epoll_fd) {
	(void)epoll_fd; // Не используется
	const char *message = "Special event occurred!\n";

	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (is_special_client(i)) { // Проверка: специальный ли клиент
			send(i, message, strlen(message), 0);
	  }
	}
}

void handle_special_clients(int client_sock, int epoll_fd) {
	uint64_t value;
	read(client_sock, &value, sizeof(value));
	notify_special_clients(epoll_fd);
}


