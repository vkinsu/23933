#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_example_server"
#define BUFFER_SIZE 1024

void to_uppercase(char *str) 
{
    while (*str) 
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
	unlink(SOCKET_PATH);
	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) 
	{
		perror("bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
    if (listen(server_fd, 5) == -1) 
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on %s\n", SOCKET_PATH);
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) 
    {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    while ((bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) 
    {
        buffer[bytes_read] = '\0';
        to_uppercase(buffer);
        printf("Received: %s\n", buffer);
    }
    if (bytes_read == -1) perror("recv");
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    printf("Server terminated.\n");
    return 0;
}

