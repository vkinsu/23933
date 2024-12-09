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
    pid_t ppid;
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
        servers_done[0] = 1;
    }
    if(sig == SIGRTMIN + 5){
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

    const struct aiocb* requests[2] = {&writerq, NULL};

    aio_write(&writerq);
    sigsend(P_PID, pd->ppid, sig);
    while(1){
        aio_suspend(requests, 1, NULL);
        int ret = aio_return(&writerq);
        if(ret == -1){
            if(errno != EINPROGRESS){
                perror("aio write error");
                return arg;
            }
        }
        else break;  
    }
    sigsend(P_PID, pd->ppid, sig + 1);
    printf("Client %d send full message\n", pd->id);

    while(servers_done[pd->id] != 1);
    printf("Client %d disconnect\n", pd->id);
    close(pd->fd);
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
    pid_t ppid = getppid();

    pthread_data pd[2];
    pd[0].fd = fds[0];
    pd[0].id = 0;
    pd[0].message = "Hello world!";
    pd[0].len = strlen(pd[0].message);
    pd[0].ppid = ppid;

    pd[1].fd = fds[1];
    pd[1].id = 1;
    pd[1].message = "Another message";
    pd[1].len = strlen(pd[1].message);
    pd[1].ppid = ppid;

    if(pthread_create(&tids[0], NULL, send_message, &pd[0]) != 0){
        perror("Thread create error");
        return 1;
    }
    if(pthread_create(&tids[1], NULL, send_message, &pd[1]) != 0){
        perror("Thread create error");
        return 1;
    }
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);
    return 0;
}