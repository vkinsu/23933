#include "server.h"
#include <stdbool.h>

void mark_client_as_special(int client_fd);
bool is_special_client(int client_fd);
void notify_special_clients(int epoll_fd);
void handle_special_clients(int client_sock, int epoll_fd);
