#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/unix_socket_server_10"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void to_uppercase(char *str) 
{
    while (*str) 
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() 
{
    int server_fd, client_fd, max_fd, activity;
    struct sockaddr_un addr;
    fd_set read_fds, temp_fds;
    char buffer[BUFFER_SIZE];
    int client_sockets[MAX_CLIENTS] = {0};

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

    if (listen(server_fd, MAX_CLIENTS) == -1) 
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    while (1) 
    {
        temp_fds = read_fds;

        activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);

        if (activity < 0) 
        {
            perror("select");
            break;
        }

        if (FD_ISSET(server_fd, &temp_fds)) 
        {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1) 
            {
                perror("accept");
                continue;
            }

            printf("New client connected\n");

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {
                if (client_sockets[i] == 0) 
                {
                    client_sockets[i] = client_fd;
                    FD_SET(client_fd, &read_fds);
                    if (client_fd > max_fd) max_fd = client_fd;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            client_fd = client_sockets[i];

            if (client_fd > 0 && FD_ISSET(client_fd, &temp_fds)) 
            {
                ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

                if (bytes_read <= 0) 
                {
                    printf("Client disconnected\n");
                    close(client_fd);
                    FD_CLR(client_fd, &read_fds);
                    client_sockets[i] = 0;
                } 
                else 
                {
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);
                    printf("%s", buffer);
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

