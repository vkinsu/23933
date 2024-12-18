typedef struct pthread_data{
    int fd;
    int id;
    char* message;
    int len;
    pid_t ppid;
} pthread_data;
