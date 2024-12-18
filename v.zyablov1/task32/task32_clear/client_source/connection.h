#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <sys/un.h>

void init_sockets(int fds[], int n);
struct sockaddr_un create_socket_address(const char* path);
int make_connect(int fd, struct sockaddr_un *addr);
void close_fds(int fds[], int n);

#endif

