#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

typedef struct pthread_data{
    int fd;
    int client_id;
} pthread_data;

int end_messages[2];
int beg_messages[2];

void begin_handler(int sig){
    if(sig == SIGRTMIN){
        beg_messages[0] = 1;
    }
    if(sig == SIGRTMIN + 3){
        beg_messages[1] = 1;
    }
}

void end_handler(int sig, siginfo_t* sig_inf, void *arg){
    if(sig == SIGRTMIN + 1){
        end_messages[0] = 1;
    }
    if(sig == SIGRTMIN + 4){
        end_messages[1] = 1;
    }
}

void* get_message(void* arg){
    pthread_data* pd = (pthread_data*)arg;
    
    struct aiocb readrq;
    memset(&readrq, 0, sizeof(readrq));
    readrq.aio_fildes = pd->fd;
    char sym;
    readrq.aio_buf = &sym;
    readrq.aio_nbytes = 1;

    while(beg_messages[pd->client_id] != 1);

    aio_read(&readrq);
    while(1){
        int ret = aio_error(&readrq);
        switch (ret){
            case 0:{
                if(end_messages[pd->client_id] == 1 && sym == EOF){
                    int client_sig = SIGRTMIN + pd->client_id * 3;
                    sigsend(P_ALL, 0, client_sig + 2);
                    close(pd->fd);
                    return NULL;
                }
                break;
            }
            default:{
                if(errno != EINPROGRESS){
                    perror("aio read error");
                    return arg;
                }
                putchar(toupper(sym));
                aio_read(&readrq);
                break;
            }
        }
    }
}

void set_reactions(){
    struct sigaction act;
    for(int id = 0; id < 2; id++){
        int sig = SIGRTMIN + id * 3;
        memset(&act, 0, sizeof(act));
        act.sa_flags |= SA_SIGINFO;
        act.sa_sigaction = end_handler;
        sigaction(sig + 1, &act, NULL);
        signal(sig, begin_handler);
    }
}

int main(int argc, char** argv){
    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(fd == -1){
        perror("Socket error");
        return 1;
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    char* socket_path = "socket";
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);

    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("Bind error");
        close(fd);
        return 1;
    }

    set_reactions();

    if(listen(fd, 2) == -1){
        perror("Listen error");
        close(fd);
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("Can't create child process");
        close(fd);
        return -1;
    }

    if(pid == 0){
        if(execvp("./client", argv) == -1){
            perror("execvp error");
            return 1;
        }
    }
    else{
        memset(end_messages, 0, sizeof(end_messages));
        pthread_t tids[2];
        pthread_data clients_data[2];

        printf("\nServer output:\n");
        int connects = 0;
        int curr_id = 0;
        while(connects < 2){
            int client_fd = accept(fd, NULL, NULL);
            if(client_fd == -1){
                continue;
            }

            clients_data[connects].fd = client_fd;
            printf("Client %d fd = %d\n", connects, client_fd);
            clients_data[connects].client_id = connects;

            if(pthread_create(&tids[connects], NULL, get_message, &clients_data[connects]) != 0){
                perror("Thread create error");
                return 1;
            }
            printf("Server start reading a message %d\n", connects);
            connects++;
        }

        pthread_join(tids[0], NULL);
        printf("Server got a message %d\n", 0);
        pthread_join(tids[1], NULL);
        printf("Server got a message %d\n", 1);
        wait(NULL);
        close(fd);
    }
    return 0;
}