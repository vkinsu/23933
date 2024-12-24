#include "connection.h"
#include <unistd.h>
#include <string.h>

void init_sockets(int fds[], int n) {
    for (int i = 0; i < n; i++) {
        fds[i] = socket(AF_UNIX, SOCK_STREAM, 0);
    }
}

struct sockaddr_un create_socket_address(const char* path) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    return addr;
}

int make_connect(int fd, struct sockaddr_un *addr) {
    if (connect(fd, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
        close(fd);
        return 1;
    }
    return 0;
}

void close_fds(int fds[], int n) {
    for (int i = 0; i < n; i++) {
        close(fds[i]);
    }
}

