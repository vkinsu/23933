#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <aio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

typedef struct pthread_data{
    int fd;
    int sig;
    char* message;
    int len;
} pthread_data;

int fds[2];

int make_connect(int fd, struct sockaddr_un *addr){
    if(connect(fd, (struct sockaddr*)addr, sizeof(*addr)) == -1){
        close(fd);
        return 1;
    }
    return 0;
}

void close_fds(int n){
    for(int i = 0; i < n; i++){
        close(fds[i]);
    }
}

int servers_done[2];

void handler(int sig){
    close(fds[sig - SIGRTMIN - 1]);
    servers_done[sig - SIGRTMIN - 1] = 1;
    signal(sig, SIG_IGN);
}

void* send_message(void* arg){
    pthread_data* pd = (pthread_data*)arg;
    struct aiocb writerq;
    memset(&writerq, 0, sizeof(writerq));
    writerq.aio_fildes = pd->fd;
    writerq.aio_buf = pd->message;
    writerq.aio_nbytes = pd->len;
    writerq.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    writerq.aio_sigevent.sigev_signo = pd->sig;

    signal(pd->sig + 2, handler);
    aio_write(&writerq);
    while(1){
        int ret = aio_return(&writerq);
        if(ret == -1){
            if(errno != EINPROGRESS){
                perror("aio write error");
                return arg;
            }
        }
        else break;  
    }

    while(servers_done[pd->sig - SIGRTMIN] != 1);
}

void init_sockets(int n){
    for(int i = 0; i < n; i++){
        fds[i] = socket(AF_UNIX, SOCK_STREAM, 0);
    }
}

int main(){
    memset(servers_done, 0, sizeof(servers_done));
    int n = 2;
    init_sockets(n);

    struct sockaddr_un addr;
    char* socket_path = "socket";
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if(make_connect(fds[1], &addr) == 1){
        perror("Connect error");
        return 1;
    }

    if(make_connect(fds[0], &addr) == 1){
        perror("Connect error");
        return 1;
    }

    pthread_t tids[2];

    pthread_data pd0;
    pd0.fd = fds[0];
    pd0.sig = SIGRTMIN;
    pd0.message = "Hello world!";
    pd0.len = strlen(pd0.message);

    pthread_data pd1;
    pd1.fd = fds[1];
    pd1.sig = SIGRTMIN + 1;
    pd1.message = "Another message";
    pd1.len = strlen(pd1.message);

    if(pthread_create(&tids[0], NULL, send_message, &pd0) != 0){
        perror("Thread create error");
        return 1;
    }
    if(pthread_create(&tids[1], NULL, send_message, &pd1) != 0){
        perror("Thread create error");
        return 1;
    }
    pthread_join(tids[0], NULL);
    printf("Client 1 send a message\n");
    pthread_join(tids[1], NULL);
    printf("Client 2 send a message\n");
    return 0;
}