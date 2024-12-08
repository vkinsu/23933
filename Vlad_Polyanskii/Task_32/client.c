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
    int id;
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
    if(sig == SIGRTMIN + 2){
        close(fds[0]);
        servers_done[0] = 1;
    }
    if(sig == SIGRTMIN + 5){
        close(fds[1]);
        servers_done[1] = 1;
    }
}

void* send_message(void* arg){
    pthread_data* pd = (pthread_data*)arg;
    int sig = SIGRTMIN + pd->id * 3;
    
    struct aiocb writerq;
    memset(&writerq, 0, sizeof(writerq));
    writerq.aio_fildes = pd->fd;
    writerq.aio_buf = pd->message;
    writerq.aio_nbytes = pd->len;
    writerq.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    writerq.aio_sigevent.sigev_signo = sig + 1;

    sigsend(P_ALL, 0, sig);
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

    while(servers_done[pd->id] != 1);
}

void init_sockets(int n){
    for(int i = 0; i < n; i++){
        fds[i] = socket(AF_UNIX, SOCK_STREAM, 0);
    }
}

void set_reactions(){
    for(int id = 0; id < 2; id++){
        int sig = SIGRTMIN + id * 3;
        signal(sig + 2, handler);
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

    set_reactions();

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
    pd0.id = 0;
    pd0.message = "Hello world!";
    pd0.len = strlen(pd0.message);

    pthread_data pd1;
    pd1.fd = fds[1];
    pd1.id = 1;
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