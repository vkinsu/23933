#include "client_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/epoll.h>

/*
int check_for_keyword(const char *buffer, const char *keyword) {
    return strstr(buffer, keyword) != NULL;
}
*/
// Функция для обработки текста
void process_text(char *buffer, ssize_t length) {
    for (int i = 0; i < length; i++) {
        buffer[i] = (char)toupper((unsigned char)buffer[i]);
    }
}

// Обработчик клиента
void handle_client(int client_sock, int epoll_fd) {
    char buffer[BUFFER_SIZE];
    
    ssize_t bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        // Если клиент отключился
        printf("Client disconnected: %d\n", client_sock);
        close(client_sock);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
    } else {
        buffer[bytes_received] = '\0';
        process_text(buffer, bytes_received);
        printf("Received from client %d: %s\n", client_sock, buffer);

        // Эхо-ответ клиенту
        send(client_sock, buffer, bytes_received, 0);
        /*  Блок ключевого слова
        // Если клиент отправил ключевое слово "keyword"
        if (strstr(buffer, "keyword") != NULL) {
            // Запускаем отправку фразы с интервалом 4 секунды
            const char *message = "Server response: keyword detected!";
            while (1) {
                // Отправляем сообщение
                send(client_sock, message, strlen(message), 0);
                sleep(4);  // Задержка 4 секунды
            }
        } else {
            // Эхо-ответ клиенту (если нет ключевого слова)
            send(client_sock, buffer, bytes_received, 0);
        }
        */
    }
}
