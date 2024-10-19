#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <string.h>
#include <termios.h>

#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"
#define BUF_SIZE 1024
//it should be /tmp/my_sockets/ but die to have no access to this directory on studying server this path is local
#define sockets_path "."

struct termios original_termios;


void recoverTermiosMode(){
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}


void setTermiosMode(){
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(recoverTermiosMode);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}


int main() {
    int client_fd;
    struct sockaddr_un server_addr;

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket.\n");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    char *cur_socket_path[BUF_SIZE];
//    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, getpid());
    snprintf(cur_socket_path, BUF_SIZE, "%s/%d", sockets_path, 0);
    strncpy(server_addr.sun_path, cur_socket_path, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect.\n");
        close(client_fd);
        exit(2);
    }

//    printf(GREEN);
    printf("Connected to the server, enter a text below:\n");
//    printf(RESET);
    setTermiosMode();

    char buf;
    while (read(STDIN_FILENO, &buf, 1) > 0) {
        if (buf == '\0') {
            break;
        }
        buf = toupper(buf);
        if(send(client_fd, &buf, 1, 0) == -1){
            perror("Send.\n");
            exit(4);
        }
//        write(client_fd, &buf, 1);
    }
    close(client_fd);
    printf("Client successfully closed.\n");
    return 0;
}