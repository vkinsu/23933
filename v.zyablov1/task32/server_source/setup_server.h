// Создание серверного сокета
int create_server_socket(const char *socket_path);

// Создание epoll и добавление серверного сокета
int setup_epoll(int server_sock);
