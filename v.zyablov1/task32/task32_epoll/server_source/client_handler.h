#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <unistd.h>
#define BUFFER_SIZE 256

void process_text(char *buffer, ssize_t length);
void handle_client(int client_sock, int epoll_fd);
int check_for_keyword(const char *buffer, const char *keyword);
void accept_new_client(int server_sock, int epoll_fd);

#endif // CLIENT_HANDLER_H

