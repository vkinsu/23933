#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"
#define BUF_SIZE 1024
//it should be /tmp/my_sockets/ but die to have no access to this directory on studying server this path is local
#define sockets_path "."


int main(){
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("Socket.\n");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    char *cur_socket_path[BUF_SIZE];
//    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, getpid());
    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, 0);
    strncpy(server_addr.sun_path, cur_socket_path, sizeof(server_addr.sun_path) - 1);

    if((bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr))) == -1){
        perror("Bind.\n");
        close(server_fd);
        exit(2);
    }

    if(listen(server_fd, 10) == -1){
        perror(("Listen.\n"));
        close(server_fd);
        exit(3);
    }

    printf(GREEN "Server is listening on %s\n" RESET, cur_socket_path);

    if((client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_len)) == -1){
        perror("Accept.\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf(GREEN "Client have connected.\n" RESET);

    char buf;
    while (1) {
        ssize_t num_bytes = recv(client_fd, &buf, 1, 0);
        if (num_bytes > 0) {
//            buf[num_bytes] = '\0';
//            for (int i = 0; i < num_bytes; ++i) {
//                buf[i] = toupper(buf[i]);
//            }
            printf("%c", buf);
        } else if (num_bytes == 0) {
            printf("Client closed connection.\n");
            break;
        } else {
            perror("recv");
            exit(EXIT_FAILURE);
        }
    }

    close(client_fd);
    close(server_fd);
    unlink(cur_socket_path);
    if(!remove(cur_socket_path))
        printf("Couldn't delete socket file %s.\n", cur_socket_path);
    return 0;
}